/**
 * stringlib.c the author's string function.
 * Author: qinfei 2015.03.11
 */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

/*获取输入的键值*/
int getch(void)
{
	struct termios oldt,newt;
	int ch;

	/*1.判断是否为标准输入*/
	if (!isatty(STDIN_FILENO)) {
		fprintf(stderr, "this problem should be run at a terminal\n");
		exit(1);
	}
	/*2.获取与终端相关的参数,将获得的信息保存在oldt变量中*/
	if(tcgetattr(STDIN_FILENO, &oldt) < 0) {
		perror("save the terminal setting");
		exit(1);
	}

	/*3.修改终端信息的结束控制字符*/
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );//使用标准输入模式、显示输入字符

	/*使用tcsetattr函数,将修改后的终端参数设置到标准输入中*/
	if(tcsetattr(STDIN_FILENO,TCSANOW, &newt) < 0) {
		perror("set terminal");
		exit(1);
	}

	/*4.读取输入的字符.*/
	ch = getchar();

	/*5.restore termial setting.*/
	if(tcsetattr(STDIN_FILENO,TCSANOW,&oldt) < 0) {
		perror("restore the termial setting");
		exit(1);
	}

	return ch;
}
