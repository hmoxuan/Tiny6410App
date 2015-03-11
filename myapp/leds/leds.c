/**
 * leds.c file implement Tiny6410.
 * Author: qinfei
 */
#include <fcntl.h>

/*Created By qinfei*/
#include <debug.h>

//Tiny6410共4个LED，编号0-3
#define MAX_TINY6410_LEDS_NUMBER 3
#define LEDS_ON  1
#define LEDS_OFF 0

//leds设备文件描述符
static int fd;

/*leds打开:打开指定的LED*/
static void Leds_On(int led_number);

/*leds关闭:关闭指定的LED*/
static void Leds_Off(int led_number);


/*leds初始化:打开设备文件、打开所有的LED*/
int Leds_Init(void)
{
	int ret = 0;

	dbg("Going into Leds_Init function!\n");

	//1.打开leds设备文件
	fd = open("/dev/leds", O_RDWR);
	if(fd < 0)
	{
		err("leds device open error ! \n ");
		return (ret = -1);
	}
	dbg("Successfully Open /dev/leds!\n");

	//2.打开所有的LED
	Leds_On(0);
	Leds_On(1);
	Leds_On(2);
	Leds_On(3);
	dbg("Opened 4 Leds in Leds_Init function!\n");
}

/*leds关闭设备文件:关闭所有的LED、释放资源*/
void Leds_Destroy(void)
{
	dbg("Going into Leds_Destroy function!\n");

	//1.关闭所有的LED
	Leds_Off(0);
	Leds_Off(1);
	Leds_Off(2);
	Leds_Off(3);

	//2.关闭leds设备文件
	close(fd);
	dbg("/dev/leds closed!\n");
}

/*leds打开:打开指定的LED*/
static void Leds_On(int led_number)
{
	dbg("Led(%d) On!\n",led_number);

	//1.判断led_number是否合法
	if(led_number > MAX_TINY6410_LEDS_NUMBER)
			led_number = 0;

	//2.打开指定的LED
	ioctl(fd, LEDS_ON, led_number);
}

/*leds关闭:关闭指定的LED*/
static void Leds_Off(int led_number)
{
	dbg("Led(%d) Off!\n",led_number);

	//1.判断led_number是否合法
	if(led_number > MAX_TINY6410_LEDS_NUMBER)
			led_number = 0;

	//2.关闭指定的LED
	ioctl(fd, LEDS_OFF, led_number);
}

/*leds应用控制:实现LED具体的应用逻辑控制*/
void Leds_AppCtl(void)
{
	dbg("Going into Leds_AppCtl function!\n");

	//1.依次关闭LED
	Leds_Off(3);
	sleep(1);

	Leds_Off(2);
	sleep(1);

	Leds_Off(1);
	sleep(1);

	Leds_Off(0);
	sleep(1);

	dbg("Turned off all Leds one by one in Leds_AppCtl function!\n");

	//中间停顿
	sleep(10);

	//2.依次打开LED
	Leds_On(3);
	sleep(1);

	Leds_On(2);
	sleep(1);

	Leds_On(1);
	sleep(1);

	Leds_On(0);
	sleep(1);

	dbg("Turned on all Leds one by one in Leds_AppCtl function!\n");
	dbg("Go out from Leds_AppCtl function!\n");
	dbg("......\n");
}
