/**
 * lcds70.c file implement Tiny6410.
 * Author: qinfei 2015.03.11
 * 
 * ���Է�����
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

/*framebuffer�豸�ļ�������*/
#define FB_DEVICE_NAME "/dev/fb0"

#define RED_COLOR565		0X0F100
#define GREEN_COLOR565		0X007E0
#define BLUE_COLOR565		0X0001F

/**
 * fb_dev�ṹ��
 * �����豸ָ�롢�ڴ�ռ�ָ�롢��������������ɫλ������Ϣ
 */
typedef struct fb_dev	{
	int fd;				/*֡�����豸Ӳ��������*/
	void *pfb;			/*ָ��֡����ӳ�䵽�û��ռ���׵�ַ*/
	int xres;			/*һ֡ͼ��Ŀ��*/
	int yres;			/*һ֡ͼ��ĸ߶�*/
	int size;			/*һ֡ͼ��Ĵ�С*/
	int bits_per_pixel;	/*ÿ�����صĴ�С*/
} fb_dev_t;

/******�������� ******/
static int fb_open(fb_dev_t *fbd, char *fbn);
static int fb_close(fb_dev_t *fbd);
static int fb_drawrect(fb_dev_t *fbd, int x0, int y0, int w, int h, int color);
static int fb_drawbmp(fb_dev_t *fbd, int x0, int y0, char *bmpn);
/**
 * ��ʼ��framebuffer�豸,�������ڴ��ַӳ��.
 * ����ֱ�ӰѴ���������ӳ�䵽framebuffer�Ļ����н�����ʾ.
 */
static int fb_open(fb_dev_t *fbd, char *fbn)
{
	struct fb_var_screeninfo vinfo;
	dbg("Going into fb_open function!\n");

	/*��fb0�豸�ļ�*/
	if((fbd->fd = open(fbn, O_RDWR)) == -1)	{
		printf("Error: Cannot open framebuffer device.\n");
		_exit(EXIT_FAILURE);
	}

	/*��ȡfb0�ɱ����*/
	ioctl(fbd->fd, FBIOGET_VSCREENINFO, &vinfo);
	fbd->xres = vinfo.xres;
	fbd->yres = vinfo.yres;
	fbd->bits_per_pixel = vinfo.bits_per_pixel;

	/*����һ֡ͼ��Ĵ�С*/
	fbd->size = fbd->xres * fbd->yres * fbd->bits_per_pixel / 8;
	printf("%d * %d,%d bits_per_pixel,screensize = %d.\n",fbd->xres,fbd->yres,fbd->bits_per_pixel,fbd->size);

	/* ��֡ӳ�䵽�ڴ�(mmap��Ӧ��)
	 * mmap���԰��ļ�����ӳ�䵽һ�������ڴ��У�ͨ��������ڴ�Ķ�ȡ���޸ģ�ʵ�ֶ��ļ��Ķ�ȡ���޸�.
	 * addr:ָ��ӳ�����ʼ��ַ��ͨ��ΪNULL����ϵͳָ�� .
	 * length:���ļ��Ķ�󳤶�ӳ�䵽�ڴ�.
	 * prot:ӳ�����ı�����ʽ�������ǿɱ�ִ��(PROT_EXEC)���ɱ�д��(PROT_WRITE)��
	 *      �ɱ���ȡ(PROT_READ)��ӳ�������ܴ�ȡ(PROT_NONE).
	 * flags:ӳ���������ԣ���ӳ������д�����ݻḴ�ƻ��ļ�������������ӳ���ļ��Ľ��̹���(MAP_SHARED)��
	 *      ��ӳ������д����������һ��ӳ��ĸ��ƣ��Դ������������޸Ĳ���д��Դ�ļ�(MAP_PRIVATE).
	 * fd:��open���ص��ļ�������������Ҫӳ����ļ�.
	 * offset:���ļ���ʼ����ƫ�ƣ������Ƿ�ҳ��С����������ͨ��Ϊ0����ʾ��ͷ��ʼӳ��.
	 * ע��:���޸�ӳ���ļ�ʱ��ֻ����ԭ�������޸ģ����������ļ����ȣ���Ϊ�ڴ����Ѿ�����õ�.
	 */
	fbd->pfb = mmap(NULL, fbd->size, PROT_READ | PROT_WRITE, MAP_SHARED, fbd->fd, 0);

	if((int)fbd->pfb == -1)	{
		printf("Error: Failed to map frambuffer device to memory!\n");
		_exit(EXIT_FAILURE);
	}

	return 0;
}

/*lcds70�ر��豸�ļ�:�ͷ���Դ*/
static int fb_close(fb_dev_t *fbd)
{
	dbg("Going into fb_close function!\n");

	/*���ӳ�� */
	munmap(fbd->pfb,fbd->size);

	/*�ر��豸�ļ�*/
	close(fbd->fd);
}

/*LCDӦ�ò��ԣ����ƾ���*/
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

/*LCDӦ�ò��ԣ���Ļ��ʾͼƬ*/
static int fb_drawbmp(fb_dev_t *fbd, int x0, int y0, char *bmpn)
{
	int x, y, x1, y1;
	bmp_t bmpt;/*�������һ��ָ���͵ı����ͻ�������⣡*/

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

/*lcds70Ӧ�ÿ���:ʵ��lcds70�����Ӧ���߼�����*/
//void lcds70_AppCtl(void)
void lcds70_AppCtl(char **argv)//argv[1]����LCD��ʾ��ͼƬ
{
	fb_dev_t *fbd;
	dbg("Going into lcds70_AppCtl function!\n");

	fbd = (fb_dev_t *)malloc(sizeof(fb_dev_t));

	fb_open(fbd, FB_DEVICE_NAME);

	/*����LED��ʾ*/
	if(fbd->bits_per_pixel == 16) //16 λɫ����
	{
		dbg("Red/Green/Blue Screen!\n");
		/*1.��ʾ������������ɫ�ľ�������*/
		fb_drawrect(fbd, 0, 0, fbd->xres, fbd->yres/3, RED_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres/3, fbd->xres, fbd->yres/3, GREEN_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres*2/3, fbd->xres, fbd->yres/3, BLUE_COLOR565);
		sleep(10);

		/*2.��ʾ������������ɫ�ľ�������*/
		dbg("Display picture in the Screen!\n");
		dbg("Command��./S_Hatress_App logo.bmp(Tiny6410 has this picture)!\n");
		fb_drawbmp(fbd, 0, 0, argv[1]);
	}
	else
		dbg("16 bits only!");

	fb_close(fbd);
	free((void *)fbd);
}

