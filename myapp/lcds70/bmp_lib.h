/**
 * bmp_lib.h file implement Tiny6410.
 * Author: qinfei 2015.03.11
 */
#ifndef __BMP_LIB_H__
#define __BMP_LIB_H__


#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif

#include <string.h>

/*bitmap��ʽ��λͼ�ļ������ 54�ֽڵ���Ϣͷ����Щ��Ϣ�ǹ̶�����ģ�����ͨ��read����ȡ*/
/*�ļ���Ϣͷ(14�ֽ�)*/
typedef struct {
	char type[2];		/*�ļ����ͣ�����Ϊ"BMP" (0X4D42)*/
	char size[4];		/*�ļ��Ĵ�С(�ֽ�)*/
	char reserved[4];	/*����������Ϊ0*/
	char off[4];		/*λͼ����������ļ�ͷ��ƫ����(�ֽ�)*/
} bmp_file_header_t;

/*λͼ��Ϣͷ(40�ֽ�)*/
typedef struct {
	char size[4];			/* ˵��BITMAPINFOHEADER �ṹ����Ҫ������ */
	char width[4];			/* λͼ���(����) */
	char height[4];			/* λͼ�߶�(����)�������ֵ��һ��������˵��ͼ���ǵ���ģ������BMP�ļ����ǵ���� */
	char planes[2];			/* Ŀ���豸��λƽ������������Ϊ1 */
	char bitcount[2];		/* ÿ�����ص�λ����1�� 4�� 8�� 16�� 24�� 32 */
	char compress[4];		/* λͼ���е�ѹ��������0��ʾ��ѹ�� */
	char img_size[4];		/* ͼ���С(�ֽ�) */
	char xpel[4];			/* ˵��ˮƽ�ֱ��ʣ��� ����/�� ��ʾ */
	char ypel[4];			/* ��ֱ */
	char clr_used[4];		/* λͼʵ��ʹ�õ���ɫ�����ɫ�� */
	char clr_important[4];	/* ��Ҫ��ɫ�����ĸ��� */
} bmp_info_header_t;

/*Ӧ�ó�����Լ���24λ��32λ��λͼ������red��green��blue��������ɫ��ռ��λ��Ӧ�õ���*/
typedef struct {
	char blue;
	char green;
	char red;
	char reserved;
} rgb_32_t;

/*��λͼ�������з�װ*/
typedef struct {
	int fd;			/* ͼ���ļ������� */
	rgb_32_t *curp;	/* ָ��ǰ�����ص� */
	int width;		/* ͼ���� */
	int height;		/* ͼ��߶� */
	int bitcount;	/* ͼ��ÿ�����ص�λ�� */
	int size;		/* ͼ���С */
	void *data;		/* ͼ����Ч����ָ�� */
} bmp_t;

/*���ڿ�����֡�����������б�����Ϊ16λ���ݱ�ʾһ�����ص�(16bpp)��������Ҫ��24λ����32λ��λͼ����ת��*/
static short transfer_to_16bit(char red,char green,char blue)
{
	return ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3);
}

/*��ȡһ����ת��Ϊ16λ������ֵ*/
static short bmp_get_pixel_16bit(bmp_t *bmp)
{
	return transfer_to_16bit(bmp->curp->red, bmp->curp->green, bmp->curp->blue);
}

/*�ַ���������εĺ���*/
static long char_to_int(char *str)
{
	return *((int *)str);
}

/*ʹbmp->curpָ����һ�����ص�*/
static void bmp_next_pixel(bmp_t *bmp)
{
    if (24 == bmp->bitcount)
	    bmp->curp = (rgb_32_t*)((int)bmp->curp + 3);
    else if (32 == bmp->bitcount)
        bmp->curp = (rgb_32_t*)((int)bmp->curp + 4);
}

/*ʹbmp->curpָ��ͼ����Ч���ݵĳ�ʼλ��*/
static void bmp_reset_pixel(bmp_t *bmp)
{
	bmp->curp = (rgb_32_t*)bmp->data;
}

extern int bmp_open(bmp_t *bmp, char *bmpn);
extern void bmp_close(bmp_t *bmp);

#ifdef __cplusplus
}
#endif

#endif  /*__BMP_LIB_H__*/

