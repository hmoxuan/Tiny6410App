/**
 *App for Tiny6410 test
 *main.c
 *Created by qinfei 20150228
 */

/*Created By qinfei*/
#include <debug.h>
#include <menu.h>
#include <leds.h>
#include <keys.h>
#include <adc.h>
#include <ds18b20.h>

/*Linux ϵͳ�豸��ʼ������*/
void System_Init(void);

int main(int argc, char **argv)
{
	int menu_num;

	dbg("Tiny6410App start ...\n");

	System_Init();//ϵͳ��ʼ��

	/*��������ѡ��˵���Ӧ����:��30�λ���ȥ���Ը���ָ�*/
	for(menu_num = 0; menu_num < 30; menu_num++)
	{
		menu_cmd();//��������ѡ��˵���Ӧ����
		printf("You have input %d times!\n",menu_num+1);
	}

	dbg("Enter into Linux System! \n");
	dbg("You can edit the System file! \n");

	return 0;
}

/*Linux ϵͳ�豸��ʼ������*/
void System_Init(void)
{
	int ret = 0;
	dbg("Going into System_Init function!\n");

	/*1.LED ��ʼ��*/
	ret = Leds_Init();
	if(ret == -1)
	{
		err("Not Open /dev/leds!\n");
	}

	/*2.keys��ʼ��*/
	ret = keys_Init();
	if(ret == -1)
	{
		err("Not Open /dev/buttons!\n");
	}

	/*3.adc��ʼ��*/
	ret = adc_Init();
	if(ret == -1)
	{
		err("Not Open /dev/adc!\n");
	}

	/*4.ds18b20��ʼ��*/
	ret = ds18b20_Init();
	if(ret == -1)
	{
		err("Not Open /dev/ds18b20!\n");
	}

	/*menu��ʼ��*/
	menu_init();
}



