/**
 * menu.c file implement Tiny6410.
 * Author: qinfei 2015.03.04
 */
#include <stdio.h>

/*Created By qinfei*/
#include <debug.h>
#include <stringlib.h>
#include <leds.h>
#include <keys.h>
#include <pwm.h>
#include <adc.h>
#include <ds18b20.h>
#include <lcds70.h>

/*menu初始化:对菜单进行必要初始化*/
void menu_init(void)
{
	dbg("Going into menu_init function!\n");
}

/*根据命令选择菜单相应操作*/
//void menu_cmd(void)
void menu_cmd(char **argv)//argv[1]传递LCD显示的图片
{
	char cmd = '0';//输入的命令

	dbg("\nGoing into menu_cmd function!\n");

	/*命令操作提示信息*/
	printf("Execute according to those command!\n");
	printf("****1:Test the Leds !\n");
	printf("****2:Test the keys !\n");
	printf("****3:Test the buzzer !\n");
	printf("****4:Test the adc !\n");
	printf("****5:Test the lcd !\n");
	printf("****6:Test the Date Timer !\n");
	printf("****7:Test the Temperature !\n");
	printf("****8:Test the usb !\n");
	printf("****q:Quit from this menu command loop !\n");
	printf("Please input your command!\n");

	/*读取输入的命令字*/
	cmd = getch();
	printf("Your command is %c!\n",cmd);

	if( cmd == '\r')
	{
		dbg("Delete the Enter Key!\n");
		return;
	}

  /*根据不同的命令执行响应的操作*/
	switch(cmd)
	{
		case '1':
			dbg("Call Test the Leds function !\n");
			Leds_AppCtl();	//实现LED具体的应用逻辑控制
			sleep(2);
			break;

		case '2':
			dbg("Call Test the keys function !\n");
			keys_AppCtl();	/*实现keys具体的应用逻辑控制*/
			sleep(2);
			break;

		case '3':
			dbg("Call Test the buzzer function !\n");
			pwm_AppCtl();	/*实现pwm具体的应用逻辑控制*/
			sleep(2);
			break;

		case '4':
			dbg("Call Test the adc function !\n");
			adc_AppCtl();	/*实现adc具体的应用逻辑控制*/
			sleep(2);
			break;

		case '5':
			dbg("Call Test the lcd function !\n");
			lcds70_AppCtl(argv); /*实现lcd具体的应用逻辑控制*/
			sleep(2);
			break;

		case '6':
			dbg("Call Test the Date Timer function !\n");
			sleep(2);
			break;

		case '7':
			dbg("Call Test the Temperature function !\n");
			ds18b20_AppCtl();	/*实现ds18b20具体的应用逻辑控制*/
			sleep(2);
			break;

		case '8':
			dbg("Call Test the usb function !\n");
			sleep(2);
			break;

		case 'q':
			dbg("Quit this loop !\n");
			sleep(2);
			break;

	  default:
			err("Your command is wrong, please input correct command!\n");
			sleep(2);
			break;
		}
}


