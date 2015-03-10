#include <linux/module.h>	/* For module specific items */
#include <linux/kernel.h>
#include <linux/fs.h>		/* For file operations */
#include <linux/init.h>		/* For __init/__exit/... */
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h> /*中断有关的头文件*/
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

#define DEVICE_NAME     "buttons"//设备名

/*按键中断设备的结构体*/
struct button_irq_desc {
    int irq;	//按键对应的中断号
    int number;	//定义键值，以传递给应用层
    char *name;	//按键名称
};

/*按键中断设备的初始化*/
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

/*开发板上按键的状态变量，注意这里是’0’，对应的ASCII码为30*/
static volatile char key_values [] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/*因为本驱动是基于中断方式的，在此创建一个等待队列，以配合中断函数使用.
 *当有按键按下并读取到键值时，将会唤醒此队列，并设置中断标志，以便能通过 read 函数判断和读取键值传递到用户态；
 *当没有按键按下时，系统并不会轮询按键状态，以节省时钟资源.
 */
static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

/*中断标识变量,配合上面的队列使用.
 *中断服务程序会把它设置为1,read 函数会把它清零*/
static volatile int ev_press = 0;

/*按键驱动的中断服务程序*/
static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
	struct button_irq_desc *button_irqs = (struct button_irq_desc *)dev_id;
	int down;		//按键按下的状态
	int number;		//按键号
	unsigned tmp;

	udelay(0);

	/*获取按键号*/
	number = button_irqs->number;

	/*根据相应的按键号,获取被按下的按键状态*/
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

	/* 状态改变，按键被按下.
	 * 当按键没有被按下时,寄存器的值为1(上拉),
	 * 但按键被按下时,寄存器对应的值为0.
	 */
	if (down != (key_values[number] & 1)) {
		/*如果key1 被按下，则key_value[0]就变为’1’，对应的ASCII 码为31*/
		key_values[number] = '0' + down;

		/*设置中断标志为1*/
        ev_press = 1;

        /*唤醒等待队列*/
        wake_up_interruptible(&button_waitq);
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}

/**
 * 在应用程序执行open(“/dev/buttons”,…)调用此函数,作用主要是注册6个按键的中断.
 * 所用的中断类型是IRQ_TYPE_EDGE_BOTH(双沿触发),在上升沿和下降沿均会产生中断，
 * 这样做是为了更加有效地判断按键状态.
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
	 * 注册中断服务程序.
	 * irq中断号
	 * buttons_interrupt中断服务程序
	 * IRQ_TYPE_EDGE_BOTH中断触发方式
	 * name中断名字
	 */
    err = request_irq(button_irqs[i].irq, buttons_interrupt, IRQ_TYPE_EDGE_BOTH,
                      button_irqs[i].name, (void *)&button_irqs[i]);
    if (err)
    	break;
    }

    /*如果出错，释放已经注册的中断并返回*/
    if (err) {
        i--;
        for (; i >= 0; i--) {
			if (button_irqs[i].irq < 0) {
				continue;
			}
			//关闭中断后程序返回
			disable_irq(button_irqs[i].irq);

			//释放中断
			free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
        }
        return -EBUSY;
    }

    /*注册成功，则中断队列标记为1，表示可以通过read读取*/
    ev_press = 1;

    return 0;
}

/*当关闭设备时，释放6个按键的中断处理函数*/
static int s3c64xx_buttons_close(struct inode *inode, struct file *file)
{
    int i;

    /*设备关闭：实际上就是free掉中断服务程序*/
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
		if (button_irqs[i].irq < 0) {
			continue;
		}
		/*释放中断号，并注销中断处理函数*/
		free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
    }

    return 0;
}

/*对应应用程序的read(fd,…)函数，主要用来向用户空间传递键值*/
static int s3c64xx_buttons_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long err;

    if (!ev_press) {
		if (filp->f_flags & O_NONBLOCK)
			/*当中断标识为0 时，并且该设备是以非阻塞方式打开时，返回*/
			return -EAGAIN;
		else
			/*当中断标识为0 时，并且该设备是以阻塞方式打开时，进入休眠状态，等待被唤醒*/
			wait_event_interruptible(button_waitq, ev_press);
    }

    /*把中断标识清零*/
    ev_press = 0;

    /*一组键值被传递到用户空间*/
    err = copy_to_user((void *)buff, (const void *)(&key_values), min(sizeof(key_values), count));

    return err ? -EFAULT : min(sizeof(key_values), count);
}

 /*判断设备的可读写状态*/
static unsigned int s3c64xx_buttons_poll( struct file *file, struct poll_table_struct *wait)
{
    unsigned int mask = 0;

    /*把调用poll或者select的进程挂入队列，以便被驱动程序唤醒*/
    poll_wait(file, &button_waitq, wait);
    if (ev_press)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

/*设备操作集
 *当应用程序操作设备文件时所调用的open,read,write等函数
 *最终会调用这个结构中的对应函数
 */
static struct file_operations dev_fops = {
    .owner   =   THIS_MODULE,//编译模块时自动创建的__this_module变量
    .open    =   s3c64xx_buttons_open,
    .release =   s3c64xx_buttons_close,
    .read    =   s3c64xx_buttons_read,
    .poll    =   s3c64xx_buttons_poll,
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

	/*把按键设备注册为misc设备，其设备号是自动分配的*/
	ret = misc_register(&misc);
	printk (DEVICE_NAME"\tinitialized\n");

	return ret;
}

/*注销设备*/
static void __exit dev_exit(void)
{
	/*注销misc混杂设备*/
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
