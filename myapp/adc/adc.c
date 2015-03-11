/**
 * adc.c file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#include <stdlib.h>

/*Created By qinfei*/
#include <debug.h>

/*adc设备文件描述符*/
static int fd;

/*adc初始化:打开设备文件*/
int adc_Init(void)
{
	int ret = 0;
	dbg("Going into adc_Init function!\n");

	fd = open("/dev/adc", 0);
	if (fd < 0)
	{
		perror("open ADC device !");
		return (ret = -1);
	}

	dbg("Successfully Open /dev/adc!\n");
	return ret;
}

/*adc关闭设备文件:释放资源*/
void adc_Destroy(void)
{
	dbg("Going into adc_Destroy function!\n");

	close(fd);
}

/*adc应用控制:实现adc具体的应用逻辑控制*/
void adc_AppCtl(void)
{
	int i = 0;
	char buffer[30];
	int len ;
	int value;

	dbg("Going into adc_AppCtl function!\n");
	printf( "\nAdc Test !\n" );
	printf( "After output 20 times, Exit this program!\n" );

	while (1)
	{
		//i++;//当输出20次数值值时，退出测试程序！

		/*1.读取AD转换值*/
		len = read(fd, buffer, sizeof(buffer) - 1);
		if (len > 0)
		{
			buffer[len] = '\0';
			sscanf(buffer, "%d", &value);
			printf("ADC Value: %d\n", value);
			i++;//当输出20次数值值时，退出测试程序！
			printf("i = %d\n", i);
		}
		else
		{
			perror("read ADC device !");
			exit(1);
		}
		if (i % 5 == 0)
			printf("\n");//每输出5行，打印一次换行！
		/*延时1s,读取下一组数据*/
		sleep(1);//等待 1s

		/*2.退出*/
		if (i >= 20)
		{
			dbg("Going out from adc_AppCtl function!\n");
			adc_Destroy();
			exit(0);
		}
	}
}
