/**
 * leds.c file implement Tiny6410.
 * Author: qinfei
 */
#include <fcntl.h>

/*Created By qinfei*/
#include <debug.h>

//Tiny6410��4��LED�����0-3
#define MAX_TINY6410_LEDS_NUMBER 3
#define LEDS_ON  1
#define LEDS_OFF 0

//leds�豸�ļ�������
static int fd;

/*leds��:��ָ����LED*/
static void Leds_On(int led_number);

/*leds�ر�:�ر�ָ����LED*/
static void Leds_Off(int led_number);


/*leds��ʼ��:���豸�ļ��������е�LED*/
int Leds_Init(void)
{
	int ret = 0;

	dbg("Going into Leds_Init function!\n");

	//1.��leds�豸�ļ�
	fd = open("/dev/leds", O_RDWR);
	if(fd < 0)
	{
		err("leds device open error ! \n ");
		return (ret = -1);
	}
	dbg("Successfully Open /dev/leds!\n");

	//2.�����е�LED
	Leds_On(0);
	Leds_On(1);
	Leds_On(2);
	Leds_On(3);
	dbg("Opened 4 Leds in Leds_Init function!\n");
}

/*leds�ر��豸�ļ�:�ر����е�LED���ͷ���Դ*/
void Leds_Destroy(void)
{
	dbg("Going into Leds_Destroy function!\n");

	//1.�ر����е�LED
	Leds_Off(0);
	Leds_Off(1);
	Leds_Off(2);
	Leds_Off(3);

	//2.�ر�leds�豸�ļ�
	close(fd);
	dbg("/dev/leds closed!\n");
}

/*leds��:��ָ����LED*/
static void Leds_On(int led_number)
{
	dbg("Led(%d) On!\n",led_number);

	//1.�ж�led_number�Ƿ�Ϸ�
	if(led_number > MAX_TINY6410_LEDS_NUMBER)
			led_number = 0;

	//2.��ָ����LED
	ioctl(fd, LEDS_ON, led_number);
}

/*leds�ر�:�ر�ָ����LED*/
static void Leds_Off(int led_number)
{
	dbg("Led(%d) Off!\n",led_number);

	//1.�ж�led_number�Ƿ�Ϸ�
	if(led_number > MAX_TINY6410_LEDS_NUMBER)
			led_number = 0;

	//2.�ر�ָ����LED
	ioctl(fd, LEDS_OFF, led_number);
}

/*ledsӦ�ÿ���:ʵ��LED�����Ӧ���߼�����*/
void Leds_AppCtl(void)
{
	dbg("Going into Leds_AppCtl function!\n");

	//1.���ιر�LED
	Leds_Off(3);
	sleep(1);

	Leds_Off(2);
	sleep(1);

	Leds_Off(1);
	sleep(1);

	Leds_Off(0);
	sleep(1);

	dbg("Turned off all Leds one by one in Leds_AppCtl function!\n");

	//�м�ͣ��
	sleep(10);

	//2.���δ�LED
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
