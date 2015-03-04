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

#define DEVICE_NAME "leds"

static long Tiny6410_leds_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		unsigned tmp;
	case 0:
	case 1:
		if (arg > 4) {
			return -EINVAL;
		}
		tmp = readl(S3C64XX_GPKDAT);
		tmp &= ~(1 << (4 + arg));
		tmp |= ( (!cmd) << (4 + arg) );
		writel(tmp, S3C64XX_GPKDAT);
		//printk (DEVICE_NAME": %d %d\n", arg, cmd);
		return 0;
		
	default:
		return -EINVAL;
	}
}

/*文件操作结构体*/
static struct file_operations dev_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= Tiny6410_leds_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

	/*设置GPIO控制寄存器，GPIO设置为输出模式,默认下灯全灭*/
	{
		unsigned tmp;
		tmp = readl(S3C64XX_GPKCON);
		tmp = (tmp & ~(0xffffU<<16))|(0x1111U<<16);
		writel(tmp, S3C64XX_GPKCON);
		
		tmp = readl(S3C64XX_GPKDAT);
		tmp |= (0xF << 4);
		writel(tmp, S3C64XX_GPKDAT);
	}

	/*注册混杂型字符设备驱动*/
	ret = misc_register(&misc);

	printk (DEVICE_NAME"\tinitialized\n");

	return ret;
}

static void __exit dev_exit(void)
{
	/*注销混杂型字符设备驱动*/
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
