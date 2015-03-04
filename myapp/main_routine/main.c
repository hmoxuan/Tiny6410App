/*
 *App for Tiny6410 test
 *main.c
 *Created by qinfei 20150228
 */
#include <stdio.h>	/*标准输入输出定义*/
#include <stdlib.h>
#include <pthread.h>		/*Linux下的多线程*/
#include <semaphore.h>	/*Linux下的信号量*/
#include <sys/wait.h>   /*Linux下等待序列相关：如wait、waitpid*/

/*Created By qinfei*/
#include <leds.h>	

int main(int argc, char **argv)
{
	int ret = 0;
	
	//dbg("Tiny6410 App start ...\n");

	/*LED 初始化*/
	ret = Leds_Init();
	if(ret == -1)
	{
		printf("Not Open /dev/leds!\n");			
	}
	else
	{
		printf("Successfully Open /dev/leds!\n");	
	}
	
	printf("Entering endless loop...\n");			
	while(1) 
	{
		sleep(60);
		
		/*实现LED具体的应用逻辑控制*/
		Leds_AppCtl();
		
		/*leds关闭设备文件*/
		Leds_Destroy();
	};
	
	return 0;
}


