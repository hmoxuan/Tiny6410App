/**
 * bmp_lib.h file implement Tiny6410.
 * Author: qinfei 2015.03.11
 */
#ifndef __BMP_LIB_H__
#define __BMP_LIB_H__


#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif

#include <string.h>

/*bitmap格式的位图文件会带有 54字节的信息头，这些信息是固定不变的，可以通过read来读取*/
/*文件信息头(14字节)*/
typedef struct {
	char type[2];		/*文件类型，必须为"BMP" (0X4D42)*/
	char size[4];		/*文件的大小(字节)*/
	char reserved[4];	/*保留，必须为0*/
	char off[4];		/*位图阵列相对于文件头的偏移量(字节)*/
} bmp_file_header_t;

/*位图信息头(40字节)*/
typedef struct {
	char size[4];			/* 说明BITMAPINFOHEADER 结构所需要的字数 */
	char width[4];			/* 位图宽度(像素) */
	char height[4];			/* 位图高度(像素)，如果该值是一个正数，说明图像是倒像的，大多数BMP文件都是倒像的 */
	char planes[2];			/* 目标设备的位平面数，必须置为1 */
	char bitcount[2];		/* 每个像素的位数，1， 4， 8， 16， 24， 32 */
	char compress[4];		/* 位图阵列的压缩方法，0表示不压缩 */
	char img_size[4];		/* 图像大小(字节) */
	char xpel[4];			/* 说明水平分辨率，用 像素/米 表示 */
	char ypel[4];			/* 垂直 */
	char clr_used[4];		/* 位图实际使用的颜色表的颜色数 */
	char clr_important[4];	/* 重要颜色索引的个数 */
} bmp_info_header_t;

/*应用程序可以兼容24位和32位的位图，其中red，green，blue这三个颜色所占的位置应该倒置*/
typedef struct {
	char blue;
	char green;
	char red;
	char reserved;
} rgb_32_t;

/*对位图操作进行封装*/
typedef struct {
	int fd;			/* 图像文件描述符 */
	rgb_32_t *curp;	/* 指向当前的像素点 */
	int width;		/* 图像宽度 */
	int height;		/* 图像高度 */
	int bitcount;	/* 图像每个像素的位数 */
	int size;		/* 图像大小 */
	void *data;		/* 图像有效数据指针 */
} bmp_t;

/*由于开发板帧缓冲在驱动中被设置为16位数据表示一个像素点(16bpp)，这里需要对24位或者32位的位图进行转化*/
static short transfer_to_16bit(char red,char green,char blue)
{
	return ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3);
}

/*获取一个被转化为16位的像素值*/
static short bmp_get_pixel_16bit(bmp_t *bmp)
{
	return transfer_to_16bit(bmp->curp->red, bmp->curp->green, bmp->curp->blue);
}

/*字符串变成整形的函数*/
static long char_to_int(char *str)
{
	return *((int *)str);
}

/*使bmp->curp指向下一个像素点*/
static void bmp_next_pixel(bmp_t *bmp)
{
    if (24 == bmp->bitcount)
	    bmp->curp = (rgb_32_t*)((int)bmp->curp + 3);
    else if (32 == bmp->bitcount)
        bmp->curp = (rgb_32_t*)((int)bmp->curp + 4);
}

/*使bmp->curp指向图像有效数据的初始位置*/
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

