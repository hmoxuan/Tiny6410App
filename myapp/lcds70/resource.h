/*
 *My Tiny6410 App
 *resource.h
 *Created by qinfei 20150315
 */
#ifndef _RESOURCE_H
#define _RESOURCE_H

/*Created By qinfei*/
#include <types.h>

#define dec10 10	//数字制式
#define hex16 16
#define dot16 16	//字体点式
#define dot24 24
#define dot32 32
#define bmp   255	//bmp格式

//----------------------------------------------------------------------
//TFT用的资源
//----------------------------------------------------------------------
int fd_fb = 0;							//帧缓冲器句柄
struct fb_var_screeninfo vinfo;			//帧缓冲器硬件信息
struct fb_fix_screeninfo finfo;			//TFT屏信息
int screen_size = 0;					//帧缓冲器长度
short *fbp565 = NULL;					//显存缓冲器地址指针

struct {
	__u16 x;							//当前X坐标
	__u16 y;							//当前Y坐标
	__u16 f;							//前景色
	__u16 b;							//背景色
	__u16 sf;							//选中时的前景色
	__u16 sb;							//选中时的背景色
	__u16 font;							//字体
}tft;

struct {
	__u16 x;							//开始x坐标
	__u16 y;							//开始Y坐标
	char name[100];						//原始文件名
	char names[100];					//选中时的文件名
}pic;

//----------------------------------------------------------------------
//字库用的资源
//----------------------------------------------------------------------
int handle_hzk32 = 0;	//字库句柄
int handle_hzk24 = 0;
int handle_hzk16 = 0;
int handle_asc32 = 0;
int handle_asc24 = 0;
int handle_asc16 = 0;

int hzk32_size = 0;		//字库长度
int hzk24_size = 0;
int hzk16_size = 0;
int asc32_size = 0;
int asc24_size = 0;
int asc16_size = 0;

char *phzk32 = NULL;	//字库指针
char *phzk24 = NULL;
char *phzk16 = NULL;
char *pasc32 = NULL;
char *pasc24 = NULL;
char *pasc16 = NULL;


#endif /* _RESOURCE_H */



