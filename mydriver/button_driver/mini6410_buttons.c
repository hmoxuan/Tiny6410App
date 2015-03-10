#include <linux/module.h>	/* For module specific items */
#include <linux/kernel.h>
#include <linux/fs.h>		/* For file operations */
#include <linux/init.h>		/* For __init/__exit/... */
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h> /*�ж��йص�ͷ�ļ�*/
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>

#include <plat/gpio-cfg.h>
#include <mach/gpio-bank-n.h>
#include <mach/gpio-bank-l.h>

#define DEVICE_NAME     "buttons"//�豸��

/*�����ж��豸�Ľṹ��*/
struct button_irq_desc {
    int irq;	//������Ӧ���жϺ�
    int number;	//�����ֵ���Դ��ݸ�Ӧ�ò�
    char *name;	//��������
};

/*�����ж��豸�ĳ�ʼ��*/
static struct button_irq_desc button_irqs [] = {
    {IRQ_EINT( 0), 0, "KEY0"},
    {IRQ_EINT( 1), 1, "KEY1"},
    {IRQ_EINT( 2), 2, "KEY2"},
    {IRQ_EINT( 3), 3, "KEY3"},
    {IRQ_EINT( 4), 4, "KEY4"},
    {IRQ_EINT( 5), 5, "KEY5"},
    {IRQ_EINT(19), 6, "KEY6"},
    {IRQ_EINT(20), 7, "KEY7"},
};

/*�������ϰ�����״̬������ע�������ǡ�0������Ӧ��ASCII��Ϊ30*/
static volatile char key_values [] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/*��Ϊ�������ǻ����жϷ�ʽ�ģ��ڴ˴���һ���ȴ����У�������жϺ���ʹ��.
 *���а������²���ȡ����ֵʱ�����ỽ�Ѵ˶��У��������жϱ�־���Ա���ͨ�� read �����жϺͶ�ȡ��ֵ���ݵ��û�̬��
 *��û�а�������ʱ��ϵͳ��������ѯ����״̬���Խ�ʡʱ����Դ.
 */
static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

/*�жϱ�ʶ����,�������Ķ���ʹ��.
 *�жϷ��������������Ϊ1,read �������������*/
static volatile int ev_press = 0;

/*�����������жϷ������*/
static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
	struct button_irq_desc *button_irqs = (struct button_irq_desc *)dev_id;
	int down;		//�������µ�״̬
	int number;		//������
	unsigned tmp;

	udelay(0);

	/*��ȡ������*/
	number = button_irqs->number;

	/*������Ӧ�İ�����,��ȡ�����µİ���״̬*/
	switch(number) {
		case 0: case 1: case 2: case 3: case 4: case 5:
			tmp = readl(S3C64XX_GPNDAT);
			down = !(tmp & (1<<number));
			break;
		case 6: case 7:
			tmp = readl(S3C64XX_GPLDAT);
			down = !(tmp & (1 << (number + 5)));
			break;
		default:
			down = 0;
	}

	/* ״̬�ı䣬����������.
	 * ������û�б�����ʱ,�Ĵ�����ֵΪ1(����),
	 * ������������ʱ,�Ĵ�����Ӧ��ֵΪ0.
	 */
	if (down != (key_values[number] & 1)) {
		/*���key1 �����£���key_value[0]�ͱ�Ϊ��1������Ӧ��ASCII ��Ϊ31*/
		key_values[number] = '0' + down;

		/*�����жϱ�־Ϊ1*/
        ev_press = 1;

        /*���ѵȴ�����*/
        wake_up_interruptible(&button_waitq);
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}

/**
 * ��Ӧ�ó���ִ��open(��/dev/buttons��,��)���ô˺���,������Ҫ��ע��6���������ж�.
 * ���õ��ж�������IRQ_TYPE_EDGE_BOTH(˫�ش���),�������غ��½��ؾ�������жϣ�
 * ��������Ϊ�˸�����Ч���жϰ���״̬.
 */
static int s3c64xx_buttons_open(struct inode *inode, struct file *file)
{
    int i;
    int err = 0;

    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
	if (button_irqs[i].irq < 0) {
		continue;
	}

	/**
	 * ע���жϷ������.
	 * irq�жϺ�
	 * buttons_interrupt�жϷ������
	 * IRQ_TYPE_EDGE_BOTH�жϴ�����ʽ
	 * name�ж�����
	 */
    err = request_irq(button_irqs[i].irq, buttons_interrupt, IRQ_TYPE_EDGE_BOTH,
                      button_irqs[i].name, (void *)&button_irqs[i]);
    if (err)
    	break;
    }

    /*��������ͷ��Ѿ�ע����жϲ�����*/
    if (err) {
        i--;
        for (; i >= 0; i--) {
			if (button_irqs[i].irq < 0) {
				continue;
			}
			//�ر��жϺ���򷵻�
			disable_irq(button_irqs[i].irq);

			//�ͷ��ж�
			free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
        }
        return -EBUSY;
    }

    /*ע��ɹ������ж϶��б��Ϊ1����ʾ����ͨ��read��ȡ*/
    ev_press = 1;

    return 0;
}

/*���ر��豸ʱ���ͷ�6���������жϴ�����*/
static int s3c64xx_buttons_close(struct inode *inode, struct file *file)
{
    int i;

    /*�豸�رգ�ʵ���Ͼ���free���жϷ������*/
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
		if (button_irqs[i].irq < 0) {
			continue;
		}
		/*�ͷ��жϺţ���ע���жϴ�����*/
		free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
    }

    return 0;
}

/*��ӦӦ�ó����read(fd,��)��������Ҫ�������û��ռ䴫�ݼ�ֵ*/
static int s3c64xx_buttons_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long err;

    if (!ev_press) {
		if (filp->f_flags & O_NONBLOCK)
			/*���жϱ�ʶΪ0 ʱ�����Ҹ��豸���Է�������ʽ��ʱ������*/
			return -EAGAIN;
		else
			/*���жϱ�ʶΪ0 ʱ�����Ҹ��豸����������ʽ��ʱ����������״̬���ȴ�������*/
			wait_event_interruptible(button_waitq, ev_press);
    }

    /*���жϱ�ʶ����*/
    ev_press = 0;

    /*һ���ֵ�����ݵ��û��ռ�*/
    err = copy_to_user((void *)buff, (const void *)(&key_values), min(sizeof(key_values), count));

    return err ? -EFAULT : min(sizeof(key_values), count);
}

 /*�ж��豸�Ŀɶ�д״̬*/
static unsigned int s3c64xx_buttons_poll( struct file *file, struct poll_table_struct *wait)
{
    unsigned int mask = 0;

    /*�ѵ���poll����select�Ľ��̹�����У��Ա㱻����������*/
    poll_wait(file, &button_waitq, wait);
    if (ev_press)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

/*�豸������
 *��Ӧ�ó�������豸�ļ�ʱ�����õ�open,read,write�Ⱥ���
 *���ջ��������ṹ�еĶ�Ӧ����
 */
static struct file_operations dev_fops = {
    .owner   =   THIS_MODULE,//����ģ��ʱ�Զ�������__this_module����
    .open    =   s3c64xx_buttons_open,
    .release =   s3c64xx_buttons_close,
    .read    =   s3c64xx_buttons_read,
    .poll    =   s3c64xx_buttons_poll,
};

/*��������豸*/
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

/*�豸��ʼ������Ҫ��ע���豸*/
static int __init dev_init(void)
{
	int ret;

	/*�Ѱ����豸ע��Ϊmisc�豸�����豸�����Զ������*/
	ret = misc_register(&misc);
	printk (DEVICE_NAME"\tinitialized\n");

	return ret;
}

/*ע���豸*/
static void __exit dev_exit(void)
{
	/*ע��misc�����豸*/
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
