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

#define DEVICE_NAME	"adc"	 /*设备节点: /dev/adc*/

static void __iomem *base_addr;

/*ADC设备的结构体*/
typedef struct {
	wait_queue_head_t wait;/*定义等待队列头*/
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

static ADC_DEV adcdev;			/*用于表示ADC设备*/
static volatile int ev_adc = 0; /*作为wait_event_interruptible的唤醒条件*/
static int adc_data;

static struct clk	*adc_clock;

/*ADC寄存器地址*/
#define __ADCREG(name)	(*(volatile unsigned long *)(base_addr + name))
#define ADCCON			__ADCREG(S3C_ADCCON)	// ADC control
#define ADCTSC			__ADCREG(S3C_ADCTSC)	// ADC touch screen control
#define ADCDLY			__ADCREG(S3C_ADCDLY)	// ADC start or Interval Delay
#define ADCDAT0			__ADCREG(S3C_ADCDAT0)	// ADC conversion data 0
#define ADCDAT1			__ADCREG(S3C_ADCDAT1)	// ADC conversion data 1
#define ADCUPDN			__ADCREG(S3C_ADCUPDN)	// Stylus Up/Down interrupt status

/*根据s3c6410的文档，设置相应的ADC控制器位*/
#define PRESCALE_DIS		(0 << 14)
#define PRESCALE_EN			(1 << 14)
#define PRSCVL(x)			((x) << 6)
#define ADC_INPUT(x)		((x) << 3)
#define ADC_START			(1 << 0)
#define ADC_ENDCVT			(1 << 15)

/*使能预分频，选择ADC通道，最后启动ADC转换*/
#define START_ADC_AIN(ch, prescale) \
	do { \
		ADCCON = PRESCALE_EN | PRSCVL(prescale) | ADC_INPUT((ch)) ; \
		ADCCON |= ADC_START; \
	} while (0)

/*ADC中断处理函数:AD转换结束后触发ADC中断，在ADC中断处理函数将数据读出，唤醒进程*/
static irqreturn_t adcdone_int_handler(int irq, void *dev_id)
{
	/*A/D转换器资源可用*/
	if (__ADC_locked) {
		/*读ADC转换结果数据*/
		adc_data = ADCDAT0 & 0x3ff;

		/*唤醒标志位，作为wait_event_interruptible的唤醒条件*/
		ev_adc = 1;
		wake_up_interruptible(&adcdev.wait);//唤醒adcdev.wait等待队列

		/*clear interrupt*/
		__raw_writel(0x0, base_addr + S3C_ADCCLRINT);
	}

	return IRQ_HANDLED;
}

/*读函数:启动AD转换，进程休眠;进程被唤醒后，将adc转换数据传给应用程序*/
static ssize_t s3c2410_adc_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
	char str[20];
	int value;
	size_t len;

	if (mini6410_adc_acquire_io() == 0) {
		/*表示A/D转换器资源可用*/
		__ADC_locked = 1;

		/*使能预分频，选择ADC通道，最后启动ADC转换*/
		START_ADC_AIN(adcdev.channel, adcdev.prescale);

		/*等待事件，当ev_adc=0时，进程被阻塞，直到ev_adc>0*/
		wait_event_interruptible(adcdev.wait, ev_adc);
		ev_adc = 0;//在adcdone_int_handler中断处理函数里，等数据读出后，ev_adc被设置为1。

		DPRINTK("AIN[%d] = 0x%04x, %d\n", adcdev.channel, adc_data, ADCCON & 0x80 ? 1:0);

		/*将在ADC中断处理函数读取的ADC转换结果赋值给value*/
		value = adc_data;

		__ADC_locked = 0;
		mini6410_adc_release_io();
	} else {
		/*如果A/D转换器资源不可用，将value赋值为-1*/
		value = -1;
	}

	/*将ADC转换结果输出到str数组里，以便传给应用空间 */
	len = sprintf(str, "%d\n", value);
	if (count >= len) {
		/*从str数组里拷贝len字节的数据到buffer，即将ADC转换数据传给应用空间 */
		int r = copy_to_user(buffer, str, len);
		return r ? r : len;
	} else {
		return -EINVAL;
	}
}

/*文件打开函数:设置模拟输入通道，设置预分频值*/
static int s3c2410_adc_open(struct inode *inode, struct file *filp)
{
	//1.初始化等待队列头
	init_waitqueue_head(&(adcdev.wait));

	//2.开发板上ADC的通道0连接着一个电位器
	adcdev.channel=0;//设置ADC的通道
	adcdev.prescale=0xff;//设置预分频值为0xff

	DPRINTK("adc opened\n");
	return 0;
}

/*文件释放函数*/
static int s3c2410_adc_release(struct inode *inode, struct file *filp)
{
	DPRINTK("adc closed\n");
	return 0;
}

/*文件操作结构体*/
static struct file_operations dev_fops = {
	owner:	THIS_MODULE,
	open:	s3c2410_adc_open,		/*文件打开函数*/
	read:	s3c2410_adc_read,		/*读函数*/
	release:	s3c2410_adc_release,/*文件释放函数*/
};

/*定义混杂设备*/
static struct miscdevice misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME,
	.fops	= &dev_fops,/*文件操作结构体*/
};

/*设备驱动模块加载函数*/
static int __init dev_init(void)
{
	int ret;

	//1.映射ADC寄存器地址将其转换为虚拟地址
	base_addr = ioremap(SAMSUNG_PA_ADC, 0x20);
	if (base_addr == NULL) {
		printk(KERN_ERR "Failed to remap register block\n");
		return -ENOMEM;
	}

	//2.获得ADC时钟并使能ADC时钟
	adc_clock = clk_get(NULL, "adc");
	if (!adc_clock) {
		printk(KERN_ERR "failed to get adc clock source\n");
		return -ENOENT;
	}
	clk_enable(adc_clock);

	/* normal ADC */
	ADCTSC = 0;

	/**
	 * 3.申请ADC中断
	 * adcdone_int_handler：中断处理函数
	 * IRQF_SHARED：共享中断
	 */
	ret = request_irq(IRQ_ADC, adcdone_int_handler, IRQF_SHARED, DEVICE_NAME, &adcdev);
	if (ret) {
		iounmap(base_addr);//取消映射
		return ret;
	}

	//4.注册混杂设备
	ret = misc_register(&misc);

	printk (DEVICE_NAME"\tinitialized\n");
	return ret;
}

/*模块卸载函数*/
static void __exit dev_exit(void)
{
	free_irq(IRQ_ADC, &adcdev);
	iounmap(base_addr);//取消映射

	//disable adc clock
	if (adc_clock) {
		clk_disable(adc_clock);
		clk_put(adc_clock);
		adc_clock = NULL;
	}

	misc_deregister(&misc);//注销混杂设备
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");

