/**
 * keys.c file implement Tiny6410.
 * Author: qinfei 2015.03.09
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

/*Created By qinfei*/
#include <debug.h>
#include <leds.h>

/*keys设备文件描述符*/
static int buttons_fd;

/*描述按键原始状态*/
static char buttons[6] = {'0', '0', '0', '0', '0', '0'};

/*keys初始化:打开设备文件*/
int keys_Init(void)
{
	int ret = 0;

	dbg("Going into keys_Init function!\n");

	//1.打开keys设备文件
	buttons_fd = open("/dev/buttons", 0);
	if(buttons_fd < 0){
		err("open device buttons error!");
		return (ret = -1);
	}
	return ret;
}

/*keys关闭设备文件:释放资源*/
void keys_Destroy(void)
{
	dbg("Going into keys_Destroy function!\n");
	close(buttons_fd);
}

/*keys应用控制:实现keys具体的应用逻辑控制*/
void keys_AppCtl(void)
{
	char current_buttons[6];
	int count_of_changed_key;
	int i;
	dbg("Going into keys_AppCtl function!\n");

	//读取键值放到current_buttons[]中
	if(read(buttons_fd, current_buttons, sizeof(current_buttons)) != sizeof(current_buttons)){
		err("read buttons:");
		exit(1);
	}

	for(i = 0, count_of_changed_key = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++){
		//若某一键值发生变化，则更新至buttons[]
		if(buttons[i] != current_buttons[i]){
			buttons[i] = current_buttons[i];
			printf("%s key %d is %s", count_of_changed_key? ", ": "", i+1, buttons[i] == '0' ? "up":"down");
			count_of_changed_key++;
		}
	}

    if(count_of_changed_key){
    	printf("\n");
    }
}
