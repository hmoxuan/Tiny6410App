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

#define DEVICE_NAME     "pwm"	//设备名

#define PWM_IOCTL_SET_FREQ		1	//定义宏变量,用于后面的 ioctl 中的控制命令
#define PWM_IOCTL_STOP			0	//定义宏变量,用于后面的 ioctl 中的控制命令

/*定义信号量lock用于互斥，该驱动程序只能同时有一个进程使用*/
static struct semaphore lock;

/* 设置 pwm 的频率，配置各个寄存器
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

	//设置GPF14为TOUT0，pwm输出
	tmp = readl(S3C64XX_GPFCON);
	tmp &= ~(0x3U << 28);
	tmp |=  (0x2U << 28);
	writel(tmp, S3C64XX_GPFCON);

	tcon = __raw_readl(S3C_TCON);	//读定时器配置寄存器TCON到tcon
	tcfg1 = __raw_readl(S3C_TCFG1);	//读取寄存器 TCFG1 到 tcfg1
	tcfg0 = __raw_readl(S3C_TCFG0);	//读取寄存器 TCFG0 到 tcfg0

	//设置TCFG0寄存器，prescaler = 50
	tcfg0 &= ~S3C_TCFG_PRESCALER0_MASK;//定时器 0 和1 的预分频值的掩码:清除TCFG[0~8]
	tcfg0 |= (50 - 1);	//设置预分频为 50

	//设置TCFG1寄存器，mux = 1/16
	tcfg1 &= ~S3C_TCFG1_MUX0_MASK;//定时器 0 分割值的掩码:清除TCFG1[0~3]
	tcfg1 |= S3C_TCFG1_MUX0_DIV16;//定时器 0 进行 16 分割

	__raw_writel(tcfg1, S3C_TCFG1);//把tcfg1的值写到分割寄存器 S3C6410_TCFG1 中
	__raw_writel(tcfg0, S3C_TCFG0);//把tcfg0的值写到预分频寄存器S3C2410_TCFG0中

	clk_p = clk_get(NULL, "pclk");//得到 pclk
	pclk  = clk_get_rate(clk_p);
	tcnt  = (pclk/50/16)/freq;//得到定时器的输入时钟，进而设置PWM的调制频率

	__raw_writel(tcnt, S3C_TCNTB(0));//PWM脉宽调制的频率等于定时器的输入时钟，确定一个计数周期的时间长度
	__raw_writel(tcnt/2, S3C_TCMPB(0));//占空比是 50%

	/**
	 * 清空低5位，其中：
	 * TCON[4] -- Dead zone enable,
	 * TCON[3] -- Timer 0 auto reload on/off,
	 * TCON[2] -- Timer 0 output inverter on/off,
	 * TCON[1] -- Timer 0 manual update,
	 * TCON[0] -- Timer 0 start/stop
	*/
	tcon &= ~0x1f;
	tcon |= 0xb;//disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
	__raw_writel(tcon, S3C_TCON);//把tcon的设置写到计数控制寄存器S3C_TCON中

	tcon &= ~2;			//clear manual update bit
	__raw_writel(tcon, S3C_TCON);
}

void PWM_Stop( void )
{
	unsigned tmp;
	tmp = readl(S3C64XX_GPFCON);//设置GPF14为输出
	tmp &= ~(0x3U << 28);
	writel(tmp, S3C64XX_GPFCON);//设置GPF14为低电平，使蜂鸣器停止
}

static int s3c64xx_pwm_open(struct inode *inode, struct file *file)
{
	/*是否获得信号量。如果是，down_trylock(&lock)=0,否则非0*/
	if (!down_trylock(&lock))
		return 0;
	else
		return -EBUSY;//返回错误信息：请求资源不可用。
}


static int s3c64xx_pwm_close(struct inode *inode, struct file *file)
{
	up(&lock);//释放信号量 lock
	return 0;
}

/*cmd是1，表示设置频率；cmd是0，表示停止pwm*/
static long s3c64xx_pwm_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		//1.如果cmd=1，进入频率设置
		case PWM_IOCTL_SET_FREQ:
			if (arg == 0)//频率参数是0,返回错误信息，表示向参数传递了无效的参数
				return -EINVAL;
			PWM_Set_Freq(arg);//设置频率
			break;

	    //2.如果cmd=0
		case PWM_IOCTL_STOP:
		default:
			PWM_Stop();//停止蜂鸣器
			break;
	}

	return 0;
}

/*初始化设备的文件操作的结构体*/
static struct file_operations dev_fops = {
    .owner			= THIS_MODULE,
    .open			= s3c64xx_pwm_open,
    .release		= s3c64xx_pwm_close,
    .unlocked_ioctl	= s3c64xx_pwm_ioctl,
};

/*定义混杂设备*/
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

/*设备初始化，主要是注册设备*/
static int __init dev_init(void)
{
	int ret;

	sema_init(&lock, 1);//初始化互斥锁
	ret = misc_register(&misc); //注册misc设备

	printk (DEVICE_NAME"\tinitialized\n");
    	return ret;
}

/*注销设备*/
static void __exit dev_exit(void)
{
	misc_deregister(&misc);//注销设备
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
MODULE_DESCRIPTION("S3C6410 PWM Driver");
