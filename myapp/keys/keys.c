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

/*keys�豸�ļ�������*/
static int buttons_fd;

/*��������ԭʼ״̬*/
static char buttons[6] = {'0', '0', '0', '0', '0', '0'};

/*keys��ʼ��:���豸�ļ�*/
int keys_Init(void)
{
	int ret = 0;

	dbg("Going into keys_Init function!\n");

	//1.��keys�豸�ļ�
	buttons_fd = open("/dev/buttons", 0);
	if(buttons_fd < 0){
		err("open device buttons error!");
		return (ret = -1);
	}
	return ret;
}

/*keys�ر��豸�ļ�:�ͷ���Դ*/
void keys_Destroy(void)
{
	dbg("Going into keys_Destroy function!\n");
	close(buttons_fd);
}

/*keysӦ�ÿ���:ʵ��keys�����Ӧ���߼�����*/
void keys_AppCtl(void)
{
	char current_buttons[6];
	int count_of_changed_key;
	int i;
	dbg("Going into keys_AppCtl function!\n");

	//��ȡ��ֵ�ŵ�current_buttons[]��
	if(read(buttons_fd, current_buttons, sizeof(current_buttons)) != sizeof(current_buttons)){
		err("read buttons:");
		exit(1);
	}

	for(i = 0, count_of_changed_key = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++){
		//��ĳһ��ֵ�����仯���������buttons[]
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