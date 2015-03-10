#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#include <plat/regs-timer.h>
#include <plat/regs-adc.h>

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define DPRINTK(x...) {printk(__FUNCTION__"(%d): ",__LINE__);printk(##x);}
#else
#define DPRINTK(x...) (void)(0)
#endif

#define DEVICE_NAME	"adc"	 /*�豸�ڵ�: /dev/adc*/

static void __iomem *base_addr;

/*ADC�豸�Ľṹ��*/
typedef struct {
	wait_queue_head_t wait;/*����ȴ�����ͷ*/
	int channel;
	int prescale;
} ADC_DEV;

#ifdef CONFIG_TOUCHSCREEN_MINI6410
extern int mini6410_adc_acquire_io(void);
extern void mini6410_adc_release_io(void);
#else
static inline int mini6410_adc_acquire_io(void) {
	return 0;
}
static inline void mini6410_adc_release_io(void) {
	/* Nothing */
}
#endif

static int __ADC_locked = 0;

static ADC_DEV adcdev;			/*���ڱ�ʾADC�豸*/
static volatile int ev_adc = 0; /*��Ϊwait_event_interruptible�Ļ�������*/
static int adc_data;

static struct clk	*adc_clock;

/*ADC�Ĵ�����ַ*/
#define __ADCREG(name)	(*(volatile unsigned long *)(base_addr + name))
#define ADCCON			__ADCREG(S3C_ADCCON)	// ADC control
#define ADCTSC			__ADCREG(S3C_ADCTSC)	// ADC touch screen control
#define ADCDLY			__ADCREG(S3C_ADCDLY)	// ADC start or Interval Delay
#define ADCDAT0			__ADCREG(S3C_ADCDAT0)	// ADC conversion data 0
#define ADCDAT1			__ADCREG(S3C_ADCDAT1)	// ADC conversion data 1
#define ADCUPDN			__ADCREG(S3C_ADCUPDN)	// Stylus Up/Down interrupt status

/*����s3c6410���ĵ���������Ӧ��ADC������λ*/
#define PRESCALE_DIS		(0 << 14)
#define PRESCALE_EN			(1 << 14)
#define PRSCVL(x)			((x) << 6)
#define ADC_INPUT(x)		((x) << 3)
#define ADC_START			(1 << 0)
#define ADC_ENDCVT			(1 << 15)

/*ʹ��Ԥ��Ƶ��ѡ��ADCͨ�����������ADCת��*/
#define START_ADC_AIN(ch, prescale) \
	do { \
		ADCCON = PRESCALE_EN | PRSCVL(prescale) | ADC_INPUT((ch)) ; \
		ADCCON |= ADC_START; \
	} while (0)

/*ADC�жϴ�����:ADת�������󴥷�ADC�жϣ���ADC�жϴ����������ݶ��������ѽ���*/
static irqreturn_t adcdone_int_handler(int irq, void *dev_id)
{
	/*A/Dת������Դ����*/
	if (__ADC_locked) {
		/*��ADCת���������*/
		adc_data = ADCDAT0 & 0x3ff;

		/*���ѱ�־λ����Ϊwait_event_interruptible�Ļ�������*/
		ev_adc = 1;
		wake_up_interruptible(&adcdev.wait);//����adcdev.wait�ȴ�����

		/*clear interrupt*/
		__raw_writel(0x0, base_addr + S3C_ADCCLRINT);
	}

	return IRQ_HANDLED;
}

/*������:����ADת������������;���̱����Ѻ󣬽�adcת�����ݴ���Ӧ�ó���*/
static ssize_t s3c2410_adc_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
	char str[20];
	int value;
	size_t len;

	if (mini6410_adc_acquire_io() == 0) {
		/*��ʾA/Dת������Դ����*/
		__ADC_locked = 1;

		/*ʹ��Ԥ��Ƶ��ѡ��ADCͨ�����������ADCת��*/
		START_ADC_AIN(adcdev.channel, adcdev.prescale);

		/*�ȴ��¼�����ev_adc=0ʱ�����̱�������ֱ��ev_adc>0*/
		wait_event_interruptible(adcdev.wait, ev_adc);
		ev_adc = 0;//��adcdone_int_handler�жϴ�����������ݶ�����ev_adc������Ϊ1��

		DPRINTK("AIN[%d] = 0x%04x, %d\n", adcdev.channel, adc_data, ADCCON & 0x80 ? 1:0);

		/*����ADC�жϴ�������ȡ��ADCת�������ֵ��value*/
		value = adc_data;

		__ADC_locked = 0;
		mini6410_adc_release_io();
	} else {
		/*���A/Dת������Դ�����ã���value��ֵΪ-1*/
		value = -1;
	}

	/*��ADCת����������str������Ա㴫��Ӧ�ÿռ� */
	len = sprintf(str, "%d\n", value);
	if (count >= len) {
		/*��str��������len�ֽڵ����ݵ�buffer������ADCת�����ݴ���Ӧ�ÿռ� */
		int r = copy_to_user(buffer, str, len);
		return r ? r : len;
	} else {
		return -EINVAL;
	}
}

/*�ļ��򿪺���:����ģ������ͨ��������Ԥ��Ƶֵ*/
static int s3c2410_adc_open(struct inode *inode, struct file *filp)
{
	//1.��ʼ���ȴ�����ͷ
	init_waitqueue_head(&(adcdev.wait));

	//2.��������ADC��ͨ��0������һ����λ��
	adcdev.channel=0;//����ADC��ͨ��
	adcdev.prescale=0xff;//����Ԥ��ƵֵΪ0xff

	DPRINTK("adc opened\n");
	return 0;
}

/*�ļ��ͷź���*/
static int s3c2410_adc_release(struct inode *inode, struct file *filp)
{
	DPRINTK("adc closed\n");
	return 0;
}

/*�ļ������ṹ��*/
static struct file_operations dev_fops = {
	owner:	THIS_MODULE,
	open:	s3c2410_adc_open,		/*�ļ��򿪺���*/
	read:	s3c2410_adc_read,		/*������*/
	release:	s3c2410_adc_release,/*�ļ��ͷź���*/
};

/*��������豸*/
static struct miscdevice misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME,
	.fops	= &dev_fops,/*�ļ������ṹ��*/
};

/*�豸����ģ����غ���*/
static int __init dev_init(void)
{
	int ret;

	//1.ӳ��ADC�Ĵ�����ַ����ת��Ϊ�����ַ
	base_addr = ioremap(SAMSUNG_PA_ADC, 0x20);
	if (base_addr == NULL) {
		printk(KERN_ERR "Failed to remap register block\n");
		return -ENOMEM;
	}

	//2.���ADCʱ�Ӳ�ʹ��ADCʱ��
	adc_clock = clk_get(NULL, "adc");
	if (!adc_clock) {
		printk(KERN_ERR "failed to get adc clock source\n");
		return -ENOENT;
	}
	clk_enable(adc_clock);

	/* normal ADC */
	ADCTSC = 0;

	/**
	 * 3.����ADC�ж�
	 * adcdone_int_handler���жϴ�����
	 * IRQF_SHARED�������ж�
	 */
	ret = request_irq(IRQ_ADC, adcdone_int_handler, IRQF_SHARED, DEVICE_NAME, &adcdev);
	if (ret) {
		iounmap(base_addr);//ȡ��ӳ��
		return ret;
	}

	//4.ע������豸
	ret = misc_register(&misc);

	printk (DEVICE_NAME"\tinitialized\n");
	return ret;
}

/*ģ��ж�غ���*/
static void __exit dev_exit(void)
{
	free_irq(IRQ_ADC, &adcdev);
	iounmap(base_addr);//ȡ��ӳ��

	//disable adc clock
	if (adc_clock) {
		clk_disable(adc_clock);
		clk_put(adc_clock);
		adc_clock = NULL;
	}

	misc_deregister(&misc);//ע�������豸
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");

