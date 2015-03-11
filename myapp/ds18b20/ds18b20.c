/**
 * ds18b20.h file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#include <stdlib.h>

/*Created By qinfei*/
#include <debug.h>

static int fd;//文件描述符

/*ds18b20初始化:打开设备文件*/
int ds18b20_Init(void)
{
	int ret = 0;
	dbg("Going into ds18b20_Init function!\n");

    fd = open("/dev/ds18b20", 0);
    if (fd < 0)
    {
        perror("open device failed !\n");
        return (ret = -1);
    }

    dbg("Successfully Open /dev/ds18b20 !\n");

    return ret;
}

/*ds18b20关闭设备文件:释放资源*/
void ds18b20_Destroy(void)
{
	dbg("Going into ds18b20_Destroy function!\n");
	close(fd);
}

/*ds18b20应用控制:实现ds18b20具体的应用逻辑控制*/
void ds18b20_AppCtl(void)
{
	int i = 0;
	unsigned char result[2];			//从ds18b20读出的结果，result[0]存放低八位
	unsigned char integer_value = 0;	//整数部分的值
	float decimal_value;				//小数部分的值
	float temperature;					//温度数值

	dbg("Going into ds18b20_AppCtl function!\n");
	printf( "\nTemperature Test !\n" );
	printf( "After output 20 times temperature,Exit this program!\n" );

	while (1)
	{
		//i++;//当输出20次温度值时，退出温度测试程序！

		//1.读取温度值放入result
		read(fd, &result, sizeof(result));

		//2.获取整数部分的值
		integer_value = ((result[0] & 0xf0) >> 4) | ((result[1] & 0x07) << 4);

		//3.获取小数部分的值(精确到0.25度)
		decimal_value = 0.5 * ((result[0] & 0x0f) >> 3) + 0.25 * ((result[0] & 0x07) >> 2);

		//4.计算温度值
		temperature = (float)integer_value + decimal_value;
		printf("The temperature is %6.2f\n",temperature);
		printf("i = %d\n", i);
		if (i % 5 == 0)
			printf("\n");//每输出5行，打印一次换行！

		/*延时1s,读取下一组数据*/
		sleep(1);//等待 1s
		
		i++;//当输出20次温度值时，退出温度测试程序！

		//5.退出
		if (i >= 20)
		{
			dbg("Going out from ds18b20_AppCtl function!\n");
			ds18b20_Destroy();
			exit(0);
		}
	}
}

