/**
 * leds.c file implement Tiny6410.
 * Author: qinfei
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

//Tiny6410共4个LED，编号0-3
#define MAX_TINY6410_LEDS_NUMBER 3
#define LEDS_ON  1
#define LEDS_OFF 0

//leds设备文件描述符
static int fd;

/*leds打开:打开指定的LED*/
static void Leds_On(char led_number);

/*leds关闭:关闭指定的LED*/
static void Leds_Off(char led_number);


/*leds初始化:打开设备文件、打开所有的LED*/
int Leds_Init(void)
{
	int ret = 0;
	char led_number = 0;
	
	//1.打开leds设备文件
	fd = open("/dev/leds", O_RDWR);
	if(fd < 0)
	{
		printf("leds device open error ! \n ");
		printf("Remember to create device node by '#mknod /dev/leds c 243 0' ! \n ");
		return (ret = -1);
	}
	
	//2.打开所有的LED
	led_number = 0;
	write(fd, &led_number, LEDS_ON);
	
	led_number = 1;
	write(fd, &led_number, LEDS_ON);
	
	led_number = 2;
	write(fd, &led_number, LEDS_ON);
	
	led_number = 3;
	write(fd, &led_number, LEDS_ON);
}

/*leds关闭设备文件:关闭所有的LED、释放资源*/
void Leds_Destroy(void)
{
	char led_number = 0;
	
	//1.打开所有的LED
	led_number = 0;
	write(fd, &led_number, LEDS_OFF);
	
	led_number = 1;
	write(fd, &led_number, LEDS_OFF);
	
	led_number = 2;
	write(fd, &led_number, LEDS_OFF);
	
	led_number = 3;
	write(fd, &led_number, LEDS_OFF);
	
	//2.关闭leds设备文件
	close(fd);
}

/*leds打开:打开指定的LED*/
static void Leds_On(char led_number)
{
	//1.判断led_number是否合法
	if(led_number > MAX_TINY6410_LEDS_NUMBER)
			led_number = 0;
			
	//2.打开指定的LED
	write(fd, &led_number, LEDS_ON);
}

/*leds关闭:关闭指定的LED*/
static void Leds_Off(char led_number)
{
	//1.判断led_number是否合法
	if(led_number > MAX_TINY6410_LEDS_NUMBER)
			led_number = 0;
			
	//2.打开指定的LED
	write(fd, &led_number, LEDS_OFF);
}

/*leds应用控制:实现LED具体的应用逻辑控制*/
void Leds_AppCtl(void)
{
	//1.依次关闭LED
	Leds_Off(3);
	sleep(5);
	
	Leds_Off(2);
	sleep(5);
	
	Leds_Off(1);
	sleep(5);
	
	Leds_Off(0);
	sleep(5);
	
	//中间停顿
	sleep(30);
	
	//2.依次打开LED
	Leds_On(3);
	sleep(5);
		
	Leds_On(2);
	sleep(5);
	
	Leds_On(1);
	sleep(5);
	
	Leds_On(0);
	sleep(5);
}
