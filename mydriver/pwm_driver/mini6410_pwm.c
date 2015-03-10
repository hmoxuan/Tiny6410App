#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <plat/regs-timer.h>
#include <mach/regs-irq.h>
#include <asm/mach/time.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>

#include <plat/gpio-cfg.h>
#include <mach/gpio-bank-e.h>
#include <mach/gpio-bank-f.h>
#include <mach/gpio-bank-k.h>

#define DEVICE_NAME     "pwm"	//�豸��

#define PWM_IOCTL_SET_FREQ		1	//��������,���ں���� ioctl �еĿ�������
#define PWM_IOCTL_STOP			0	//��������,���ں���� ioctl �еĿ�������

/*�����ź���lock���ڻ��⣬����������ֻ��ͬʱ��һ������ʹ��*/
static struct semaphore lock;

/* ���� pwm ��Ƶ�ʣ����ø����Ĵ���
 * freq:pclk/50/16/65536 ~ pclk/50/16
 * if pclk=50MHz, freq is 1Hz to 62500Hz
 * human ear:20Hz~20000Hz
 */
static void PWM_Set_Freq( unsigned long freq )
{
	unsigned long tcon;
	unsigned long tcnt;
	unsigned long tcfg1;
	unsigned long tcfg0;

	struct clk *clk_p;
	unsigned long pclk;

	unsigned tmp;

	//����GPF14ΪTOUT0��pwm���
	tmp = readl(S3C64XX_GPFCON);
	tmp &= ~(0x3U << 28);
	tmp |=  (0x2U << 28);
	writel(tmp, S3C64XX_GPFCON);

	tcon = __raw_readl(S3C_TCON);	//����ʱ�����üĴ���TCON��tcon
	tcfg1 = __raw_readl(S3C_TCFG1);	//��ȡ�Ĵ��� TCFG1 �� tcfg1
	tcfg0 = __raw_readl(S3C_TCFG0);	//��ȡ�Ĵ��� TCFG0 �� tcfg0

	//����TCFG0�Ĵ�����prescaler = 50
	tcfg0 &= ~S3C_TCFG_PRESCALER0_MASK;//��ʱ�� 0 ��1 ��Ԥ��Ƶֵ������:���TCFG[0~8]
	tcfg0 |= (50 - 1);	//����Ԥ��ƵΪ 50

	//����TCFG1�Ĵ�����mux = 1/16
	tcfg1 &= ~S3C_TCFG1_MUX0_MASK;//��ʱ�� 0 �ָ�ֵ������:���TCFG1[0~3]
	tcfg1 |= S3C_TCFG1_MUX0_DIV16;//��ʱ�� 0 ���� 16 �ָ�

	__raw_writel(tcfg1, S3C_TCFG1);//��tcfg1��ֵд���ָ�Ĵ��� S3C6410_TCFG1 ��
	__raw_writel(tcfg0, S3C_TCFG0);//��tcfg0��ֵд��Ԥ��Ƶ�Ĵ���S3C2410_TCFG0��

	clk_p = clk_get(NULL, "pclk");//�õ� pclk
	pclk  = clk_get_rate(clk_p);
	tcnt  = (pclk/50/16)/freq;//�õ���ʱ��������ʱ�ӣ���������PWM�ĵ���Ƶ��

	__raw_writel(tcnt, S3C_TCNTB(0));//PWM������Ƶ�Ƶ�ʵ��ڶ�ʱ��������ʱ�ӣ�ȷ��һ���������ڵ�ʱ�䳤��
	__raw_writel(tcnt/2, S3C_TCMPB(0));//ռ�ձ��� 50%

	/**
	 * ��յ�5λ�����У�
	 * TCON[4] -- Dead zone enable,
	 * TCON[3] -- Timer 0 auto reload on/off,
	 * TCON[2] -- Timer 0 output inverter on/off,
	 * TCON[1] -- Timer 0 manual update,
	 * TCON[0] -- Timer 0 start/stop
	*/
	tcon &= ~0x1f;
	tcon |= 0xb;//disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
	__raw_writel(tcon, S3C_TCON);//��tcon������д���������ƼĴ���S3C_TCON��

	tcon &= ~2;			//clear manual update bit
	__raw_writel(tcon, S3C_TCON);
}

void PWM_Stop( void )
{
	unsigned tmp;
	tmp = readl(S3C64XX_GPFCON);//����GPF14Ϊ���
	tmp &= ~(0x3U << 28);
	writel(tmp, S3C64XX_GPFCON);//����GPF14Ϊ�͵�ƽ��ʹ������ֹͣ
}

static int s3c64xx_pwm_open(struct inode *inode, struct file *file)
{
	/*�Ƿ����ź���������ǣ�down_trylock(&lock)=0,�����0*/
	if (!down_trylock(&lock))
		return 0;
	else
		return -EBUSY;//���ش�����Ϣ��������Դ�����á�
}


static int s3c64xx_pwm_close(struct inode *inode, struct file *file)
{
	up(&lock);//�ͷ��ź��� lock
	return 0;
}

/*cmd��1����ʾ����Ƶ�ʣ�cmd��0����ʾֹͣpwm*/
static long s3c64xx_pwm_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		//1.���cmd=1������Ƶ������
		case PWM_IOCTL_SET_FREQ:
			if (arg == 0)//Ƶ�ʲ�����0,���ش�����Ϣ����ʾ�������������Ч�Ĳ���
				return -EINVAL;
			PWM_Set_Freq(arg);//����Ƶ��
			break;

	    //2.���cmd=0
		case PWM_IOCTL_STOP:
		default:
			PWM_Stop();//ֹͣ������
			break;
	}

	return 0;
}

/*��ʼ���豸���ļ������Ľṹ��*/
static struct file_operations dev_fops = {
    .owner			= THIS_MODULE,
    .open			= s3c64xx_pwm_open,
    .release		= s3c64xx_pwm_close,
    .unlocked_ioctl	= s3c64xx_pwm_ioctl,
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

	sema_init(&lock, 1);//��ʼ��������
	ret = misc_register(&misc); //ע��misc�豸

	printk (DEVICE_NAME"\tinitialized\n");
    	return ret;
}

/*ע���豸*/
static void __exit dev_exit(void)
{
	misc_deregister(&misc);//ע���豸
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
MODULE_DESCRIPTION("S3C6410 PWM Driver");
