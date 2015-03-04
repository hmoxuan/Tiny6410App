/**
 * mini6410_leds.c file implement Tiny6410.
 * Author: qinfei 2015.03.04
 */

#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
//#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>

/* Tiny6410 board related header files */
#include <plat/gpio-cfg.h>
#include <mach/gpio-bank-e.h>
#include <mach/gpio-bank-k.h>

#define DEVICE_NAME "leds"	//�豸��

/* ͨ��ioctl,����LED */
static long Tiny6410_leds_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned tmp;
	
	//�����������LED������
	switch(cmd) {
		case 0:
		case 1:
			if (arg > 4) {//��4��LED�ƣ����1-4
				return -EINVAL;
			}
			//GPK4-7-->LED1-4
			tmp = readl(S3C64XX_GPKDAT);	 //��ȡGPKDAT��ԭʼ����
			tmp &= ~(1 << (4 + arg));      //LED���1-4ת������Ӧ��GPK4-7λ�������Ӧλ
			tmp |= ( (!cmd) << (4 + arg) );//�����������ö�Ӧλ
			writel(tmp, S3C64XX_GPKDAT);   //д��GPKDAT��������
			//printk (DEVICE_NAME": %d %d\n", arg, cmd);
			return 0;
			
		default:
			return -EINVAL;
	}
}

/*�ļ������ṹ��*/
static struct file_operations dev_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= Tiny6410_leds_ioctl,//ͨ��ioctl,����LED
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,/*�ļ������ṹ��*/
};

/*ģ���ʼ��*/
static int __init dev_init(void)
{
	int ret;

	/*����GPIO���ƼĴ�����GPIO����Ϊ���ģʽ,Ĭ���µ�ȫ��*/
	{
		unsigned tmp;
		tmp = readl(S3C64XX_GPKCON);
		tmp = (tmp & ~(0xffffU<<16))|(0x1111U<<16);
		writel(tmp, S3C64XX_GPKCON);
		
		tmp = readl(S3C64XX_GPKDAT);
		tmp |= (0xF << 4);
		writel(tmp, S3C64XX_GPKDAT);
	}

	/*ע��������ַ��豸����*/
	ret = misc_register(&misc);

	printk (DEVICE_NAME"\tinitialized\n");

	return ret;
}

/*ģ��ж��*/
static void __exit dev_exit(void)
{
	/*ע���������ַ��豸����*/
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
