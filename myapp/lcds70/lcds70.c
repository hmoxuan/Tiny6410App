/**
 * lcds70.c file implement Tiny6410.
 * Author: qinfei 2015.03.11
 * 
 * 测试方法：
 * ./S_Hatress_App logo.bmp(Tiny6410 has this picture)!
 * 
 */
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

/*Created By qinfei*/
#include <debug.h>
#include <bmp_lib.h>

/*framebuffer设备文件描述符*/
#define FB_DEVICE_NAME "/dev/fb0"

#define RED_COLOR565		0X0F100
#define GREEN_COLOR565		0X007E0
#define BLUE_COLOR565		0X0001F

/**
 * fb_dev结构体
 * 包含设备指针、内存空间指针、横竖像素数、颜色位数等信息
 */
typedef struct fb_dev	{
	int fd;				/*帧缓冲设备硬件描述符*/
	void *pfb;			/*指向帧缓冲映射到用户空间的首地址*/
	int xres;			/*一帧图像的宽度*/
	int yres;			/*一帧图像的高度*/
	int size;			/*一帧图像的大小*/
	int bits_per_pixel;	/*每个像素的大小*/
} fb_dev_t;

/******函数声明 ******/
static int fb_open(fb_dev_t *fbd, char *fbn);
static int fb_close(fb_dev_t *fbd);
static int fb_drawrect(fb_dev_t *fbd, int x0, int y0, int w, int h, int color);
static int fb_drawbmp(fb_dev_t *fbd, int x0, int y0, char *bmpn);
/**
 * 初始化framebuffer设备,并进行内存地址映射.
 * 可以直接把处理后的数据映射到framebuffer的缓存中进行显示.
 */
static int fb_open(fb_dev_t *fbd, char *fbn)
{
	struct fb_var_screeninfo vinfo;
	dbg("Going into fb_open function!\n");

	/*打开fb0设备文件*/
	if((fbd->fd = open(fbn, O_RDWR)) == -1)	{
		printf("Error: Cannot open framebuffer device.\n");
		_exit(EXIT_FAILURE);
	}

	/*获取fb0可变参数*/
	ioctl(fbd->fd, FBIOGET_VSCREENINFO, &vinfo);
	fbd->xres = vinfo.xres;
	fbd->yres = vinfo.yres;
	fbd->bits_per_pixel = vinfo.bits_per_pixel;

	/*计算一帧图像的大小*/
	fbd->size = fbd->xres * fbd->yres * fbd->bits_per_pixel / 8;
	printf("%d * %d,%d bits_per_pixel,screensize = %d.\n",fbd->xres,fbd->yres,fbd->bits_per_pixel,fbd->size);

	/* 将帧映射到内存(mmap的应用)
	 * mmap可以把文件内容映射到一段虚拟内存中，通过对这段内存的读取和修改，实现对文件的读取和修改.
	 * addr:指定映射的起始地址，通常为NULL，由系统指定 .
	 * length:将文件的多大长度映射到内存.
	 * prot:映射区的保护方式，可以是可被执行(PROT_EXEC)，可被写入(PROT_WRITE)，
	 *      可被读取(PROT_READ)，映射区不能存取(PROT_NONE).
	 * flags:映射区的特性，对映射区的写入数据会复制回文件，且允许其他映射文件的进程共享(MAP_SHARED)，
	 *      对映射区的写入操作会产生一个映射的复制，对此区域所做的修改不会写会源文件(MAP_PRIVATE).
	 * fd:由open返回的文件描述符，代表要映射的文件.
	 * offset:以文件开始出的偏移，必须是分页大小的整数倍，通常为0，表示从头开始映射.
	 * 注意:在修改映射文件时，只能在原长度上修改，不能增加文件长度，因为内存是已经分配好的.
	 */
	fbd->pfb = mmap(NULL, fbd->size, PROT_READ | PROT_WRITE, MAP_SHARED, fbd->fd, 0);

	if((int)fbd->pfb == -1)	{
		printf("Error: Failed to map frambuffer device to memory!\n");
		_exit(EXIT_FAILURE);
	}

	return 0;
}

/*lcds70关闭设备文件:释放资源*/
static int fb_close(fb_dev_t *fbd)
{
	dbg("Going into fb_close function!\n");

	/*解除映射 */
	munmap(fbd->pfb,fbd->size);

	/*关闭设备文件*/
	close(fbd->fd);
}

/*LCD应用测试：绘制矩形*/
static int fb_drawrect(fb_dev_t *fbd, int x0, int y0, int w, int h, int color)
{
	int x,y;
	dbg("Going into fb_drawrect function!\n");

	for(y=y0; y<y0+h; y++)
	{
		for(x=x0; x<x0+w; x++)
		{
			*((short *)(fbd->pfb) + y*fbd->xres +x) = color;
		}
	}
	return 0;
}

/*LCD应用测试：屏幕显示图片*/
static int fb_drawbmp(fb_dev_t *fbd, int x0, int y0, char *bmpn)
{
	int x, y, x1, y1;
	bmp_t bmpt;/*如果定义一个指针型的变量就会出现问题！*/

	bmp_open(&bmpt, bmpn);

	x1 = x0 + bmpt.width;
	y1 = y0 + bmpt.height;

	for(y=y1; y>y0; y--)
	{
		for(x=x0; x<x1; x++)
		{
			if(x1>fbd->xres || y1>fbd->yres)
			{
				bmp_next_pixel(&bmpt);
				continue;
			}

			*((short *)(fbd->pfb) + y*fbd->xres + x) = bmp_get_pixel_16bit(&bmpt);
			bmp_next_pixel(&bmpt);
		}
	}

	bmp_close(&bmpt);

	return 0;
}

/*lcds70应用控制:实现lcds70具体的应用逻辑控制*/
//void lcds70_AppCtl(void)
void lcds70_AppCtl(char **argv)//argv[1]传递LCD显示的图片
{
	fb_dev_t *fbd;
	dbg("Going into lcds70_AppCtl function!\n");

	fbd = (fb_dev_t *)malloc(sizeof(fb_dev_t));

	fb_open(fbd, FB_DEVICE_NAME);

	/*测试LED显示*/
	if(fbd->bits_per_pixel == 16) //16 位色的屏
	{
		dbg("Red/Green/Blue Screen!\n");
		/*1.显示红绿蓝三种颜色的矩形区域*/
		fb_drawrect(fbd, 0, 0, fbd->xres, fbd->yres/3, RED_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres/3, fbd->xres, fbd->yres/3, GREEN_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres*2/3, fbd->xres, fbd->yres/3, BLUE_COLOR565);
		sleep(10);

		/*2.显示红绿蓝三种颜色的矩形区域*/
		dbg("Display picture in the Screen!\n");
		dbg("Command：./S_Hatress_App logo.bmp(Tiny6410 has this picture)!\n");
		fb_drawbmp(fbd, 0, 0, argv[1]);
	}
	else
		dbg("16 bits only!");

	fb_close(fbd);
	free((void *)fbd);
}

