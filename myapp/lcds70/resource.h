/*
 *My Tiny6410 App
 *resource.h
 *Created by qinfei 20150315
 */
#ifndef _RESOURCE_H
#define _RESOURCE_H

/*Created By qinfei*/
#include <types.h>

#define dec10 10	//������ʽ
#define hex16 16
#define dot16 16	//�����ʽ
#define dot24 24
#define dot32 32
#define bmp   255	//bmp��ʽ

//----------------------------------------------------------------------
//TFT�õ���Դ
//----------------------------------------------------------------------
int fd_fb = 0;							//֡���������
struct fb_var_screeninfo vinfo;			//֡������Ӳ����Ϣ
struct fb_fix_screeninfo finfo;			//TFT����Ϣ
int screen_size = 0;					//֡����������
short *fbp565 = NULL;					//�Դ滺������ַָ��

struct {
	__u16 x;							//��ǰX����
	__u16 y;							//��ǰY����
	__u16 f;							//ǰ��ɫ
	__u16 b;							//����ɫ
	__u16 sf;							//ѡ��ʱ��ǰ��ɫ
	__u16 sb;							//ѡ��ʱ�ı���ɫ
	__u16 font;							//����
}tft;

struct {
	__u16 x;							//��ʼx����
	__u16 y;							//��ʼY����
	char name[100];						//ԭʼ�ļ���
	char names[100];					//ѡ��ʱ���ļ���
}pic;

//----------------------------------------------------------------------
//�ֿ��õ���Դ
//----------------------------------------------------------------------
int handle_hzk32 = 0;	//�ֿ���
int handle_hzk24 = 0;
int handle_hzk16 = 0;
int handle_asc32 = 0;
int handle_asc24 = 0;
int handle_asc16 = 0;

int hzk32_size = 0;		//�ֿⳤ��
int hzk24_size = 0;
int hzk16_size = 0;
int asc32_size = 0;
int asc24_size = 0;
int asc16_size = 0;

char *phzk32 = NULL;	//�ֿ�ָ��
char *phzk24 = NULL;
char *phzk16 = NULL;
char *pasc32 = NULL;
char *pasc24 = NULL;
char *pasc16 = NULL;


#endif /* _RESOURCE_H */



