/**
 * stringlib.c the author's string function.
 * Author: qinfei 2015.03.11
 */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

/*��ȡ����ļ�ֵ*/
int getch(void)
{
	struct termios oldt,newt;
	int ch;

	/*1.�ж��Ƿ�Ϊ��׼����*/
	if (!isatty(STDIN_FILENO)) {
		fprintf(stderr, "this problem should be run at a terminal\n");
		exit(1);
	}
	/*2.��ȡ���ն���صĲ���,����õ���Ϣ������oldt������*/
	if(tcgetattr(STDIN_FILENO, &oldt) < 0) {
		perror("save the terminal setting");
		exit(1);
	}

	/*3.�޸��ն���Ϣ�Ľ��������ַ�*/
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );//ʹ�ñ�׼����ģʽ����ʾ�����ַ�

	/*ʹ��tcsetattr����,���޸ĺ���ն˲������õ���׼������*/
	if(tcsetattr(STDIN_FILENO,TCSANOW, &newt) < 0) {
		perror("set terminal");
		exit(1);
	}

	/*4.��ȡ������ַ�.*/
	ch = getchar();

	/*5.restore termial setting.*/
	if(tcsetattr(STDIN_FILENO,TCSANOW,&oldt) < 0) {
		perror("restore the termial setting");
		exit(1);
	}

	return ch;
}
