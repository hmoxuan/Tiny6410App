/**
 * adc.c file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>

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
	char buffer[30];
	int len ;
	int value;

	dbg("Going into adc_AppCtl function!\n");

	len = read(fd, buffer, sizeof(buffer) - 1);
	if (len > 0)
	{
		buffer[len] = '\0';
		sscanf(buffer, "%d", &value);
		printf("ADC Value: %d\n", value);
	}
	else
	{
		perror("read ADC device !");
		exit(1);
	}
	sleep(1);
}
