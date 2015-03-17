/**
 * lcds70.c file implement Tiny6410.
 * Author: qinfei 2015.03.11
 * 
 * 测试方法：
 * ./S_Hatress_App logo.bmp(Tiny6410 has this picture)!
 * 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

/*Created By qinfei*/
#include <types.h>			//数据类型
#include "resource.h"		//资源文件
#include "database.h"		//数据库
#include "lcds70.h"			//函数声明

//**********************************************************************
//主函数
//**********************************************************************
//int lcds70_AppCtl(char **argv)
int lcds70_AppCtl(void)
{
	int i;
	printf("Start to test LCD......\n");

	/*1.TFT屏硬件内存映射*/
	if(FbInit())//TFT出错
	{
		return(1);
	}
	printf("Init the LCD !\n");
	sleep(2);

	/*2.映射汉字库点阵*/
	if(DotInit())//有字库出错
	{
		return(2);
	}
	printf("background value = black \n");
	sleep(2);

	/*3.用指定颜色清屏*/
	FbClear(0);
	printf("Clear LCD Screen!\n");
	sleep(2);

    /*4.显示图形：sw==0显示原图,非则显示选中时的图形*/
	Dispicture(0,0,mainbmp,0);
	printf("Display main.bmp !\n");
	sleep(2);

	Dispicture(800-300,480-200,mainbmp,1);
	printf("Display bootlogo.bmp !\n");
	sleep(2);

	/*5.显示文本混合字符串*/
	for(i=0;i<5;i++)
		DispString(mainmenu[i]);
	printf("Display menu!\n");

    /*6.关闭TFT屏句柄与内存映射*/
    FClose();			//关闭句柄与映射的内存

	return(0);
}


//-------------------------------------------------------------------
//显示图形
//sw==0显示原图 非则显示选中时的图形
//x:x的起始坐标；y:y的起始坐标。
//*p：图片文件名指针
//-------------------------------------------------------------------
void Dispicture(int x,int y,const char *p,int sw)
{
	int i,k,form;

	pic.x=x;//x的开始坐标
	pic.y=y;//y的开始坐标

	/*1.常数字符串转换成整数*/
	form=StrInt(p,1,dec10);

	/*2.获取图片名*/
	memset(pic.name,0,100);//内存空间初始化为0
	for(i=2,k=0;p[i]!=',';i++,k++)
		pic.name[k]=p[i];//原图文件名

	/*3.获取选中时文件名*/
	if(form)
	{
		memset(pic.names,0,100);//内存空间初始化为0
		i++;//跳过','
		for(k=0;p[i]!=0;i++,k++)
			pic.names[k]=p[i];//选中时文件名
	}

	printf("%d-%d-%d\n",pic.x,pic.y,form);
	printf("%s\n",pic.name);
	printf("%s\n",pic.names);

	/*4.显示bmp图形*/
	DispBmp(sw);
}


//-------------------------------------------------------------------
//显示bmp图形
//sw==0显示原图,非则显示选中时的图形
//-------------------------------------------------------------------
int DispBmp(int sw)
{
	struct stat x;
	int handle;
	char *paddr;
	int offset,resx,resy,h,v,n,k;
	__u16 m;

//hzk32
	/*1.打开图片文件*/
	if(sw==0)
	{
		//打开图片文件
		handle = open(pic.name, O_RDONLY);
		if (!handle)
		{
			printf("Error: cannot open file %s.\n",pic.name);
			return(1);//不能打开1
		}
	}
	else
	{
		//获取选中时的文件名
		handle = open(pic.names, O_RDONLY);
		if (!handle)
		{
			printf("Error: cannot open file %s.\n",pic.names);
			return(2);//不能打开2
		}
	}

	/*2.获取图片文件大小*/
	fstat(handle,&x);//取文件大小

	/*3.建立与内存的映射*/
	paddr = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle, 0);//建立与内存的映射
    if (paddr == MAP_FAILED) //建立与内存的映射失败
    {
		if(sw==0)
			printf("Error: failed to map %s to memory.\n",pic.name);
		else
			printf("Error: failed to map %s to memory.\n",pic.names);
        return(3);
    }

    /*4.计算图片大小*/
	offset=*(paddr+0x0a+1);
	offset<<=8;
	offset|=*(paddr+0x0a+0);

	resx=*(paddr+0x12+1);
	resx<<=8;
	resx|=*(paddr+0x12+0);

	resy=*(paddr+0x16+1);
	resy<<=8;
	resy|=*(paddr+0x16+0);

	printf("BMPSIZE=%d offset=%x resx=%x resy=%x\n",x.st_size,offset,resx,resy);

	 /*5.显示图片*/
	for(v=0;v<resy;v++)
	{
		n = offset + v * resx * 2;	//每一行的bmp地址
		k = pic.y + resy - 1 - v;
		if((k>=0) && (k<vinfo.yres))//行在可视区域内
		{
			k *= vinfo.xres;//每一行的屏幕地址
			for(h=0;h<resx;h++)
			{
				m=*(paddr+n+h+h+1);
				m<<=8;
				m|=*(paddr+n+h+h+0);//一个点
				if((h+pic.x) < vinfo.xres)	//列在可视区域内
					*(fbp565+k+h+pic.x)=m;
			}
		}
	}

	/*6.释放内存、关闭文件bmp*/
	munmap(paddr,x.st_size);//释放内存
	close(handle);//关闭bmp文件
	return 0;
}


//-------------------------------------------------------------------
//显示文本混合字符串
//格式：行,列,前景色,背景色,选中前景色,选中背景色,字体大小,字符串,0x00结束
//-------------------------------------------------------------------
void DispString(const char *p)
{
	int c1;

	/*取文本的H，V，前景色，背景色，字体 */
	GetHVColor(p);//取文本的H，V，前景色，背景色，字体
	p+=33;//文本区首址

	while(1)
	{
		c1=*p;
		p++;//1.指向下一个文本
		if(c1 == 0)//结束符
			return;

		if(c1<0xa1)//2.是ASCII
		{
			if(c1>0x7f)//盲区
				c1=' ';//按space显示
		}
		else//3.可能是汉字
		{
			if(*p<0xa1)//第二个是ASCII
				c1=' ';//组不成汉字码只能按space显示
			else
			{
				c1<<=8;
				c1|=(*p);
				p++;//一个汉字二个ASCII位置
			}
		}

		DispDot(c1);//4.显示点阵
	}
}


//-------------------------------------------------------------------
//显示点阵
//i高8位为0：是ASCII，非则即是汉字。
//-------------------------------------------------------------------
void DispDot(__u16 i)
{
	int h,v,leng,w,mask;//行长，列长，占用字节数,行占用字节,掩码
	char *addr;
	int tftaddr;
	unsigned int x,m,n;

	if(tft.font==dot16)//16点阵
	{
		if(i&0xff00)
		{
			h=v=16;//汉字
			leng=32;
			w=2;
			mask=0x8000;
			i-=0xa1a1;
			m=i>>8;
			m*=94;
			m+=(i&0xff);
			i=m;
			addr=phzk16;
		}
		else
		{
			h=8;//ASCII
			v=16;
			leng=16;
			w=1;
			mask=0x80;
			addr=pasc16;
		}
	}
	else//24点阵或别的点阵
	{
		if(tft.font==dot32)//32点阵
		{
			if(i&0xff00)
			{
				h=v=32;//汉字
				leng=128;
				w=4;
				mask=0x80000000;
				i-=0xa1a1;
				m=i>>8;
				m*=94;
				m+=(i&0xff);
				i=m;
				addr=phzk32;
			}
			else
			{
				h=16;//ASCII
				v=32;
				leng=64;
				w=2;
				mask=0x8000;
				addr=pasc32;
			}
		}
		else//24点阵(默认)
		{
			if(i&0xff00)
			{
				h=v=24;//汉字
				leng=72;
				w=3;
				mask=0x800000;
				i-=0xa1a1;
				m=i>>8;
				m*=94;
				m+=(i&0xff);
				i=m;
				addr=phzk24;
			}
			else
			{
				h=12;//ASCII
				v=24;
				leng=48;
				w=2;
				mask=0x8000;
				addr=pasc24;
			}
		}
	}
	addr+=(i*leng);//对应的地址
	tftaddr = tft.y*vinfo.xres + tft.x;//屏幕位置

	x=0;//从0行开始
	while(x<v)
	{
		n=0;
		for(m=0;m<w;m++)
		{
			n<<=8;
			n|=*(addr+m);//合并每一行的点阵(最多32点陈)
		}
		addr+=w;//指向下一行
		for(m=0;m<h;m++)//从0列开始(每行的列)
		{
			if(n&mask)
				*(fbp565+tftaddr+m)=tft.f;
			else
				*(fbp565+tftaddr+m)=tft.b;
			n<<=1;//指向下一列
		}
		tftaddr+=vinfo.xres;//屏幕指向下一行
		x++;//行计数+1
	}
	tft.x+=h;//列加
	if(tft.x>=vinfo.xres)
	{
		tft.x=0;
		tft.y+=v;//行加
		if(tft.y>=vinfo.yres)
			tft.y=0;
	}
}


//-------------------------------------------------------------------
//取文本的H，V，前景色，背景色,字体
//-------------------------------------------------------------------
void GetHVColor(const void *p)
{
	tft.x=StrInt(p,4,dec10);		//行
	tft.y=StrInt((p+5),4,dec10);	//列
	tft.f=StrInt((p+10),4,hex16);	//前景色
	tft.b=StrInt((p+15),4,hex16);	//背景色
	tft.sf=StrInt((p+20),4,hex16);	//选中时的前景色
	tft.sb=StrInt((p+25),4,hex16);	//选中时的背景色
	tft.font=StrInt((p+30),2,dec10);//字体
}


//-------------------------------------------------------------------
//常数字符串转换成整数：leng=字符串长度
//-------------------------------------------------------------------
int StrInt(const void *src,char leng,int mode)
{
	int i,k,l,x;
	char buf[leng];

	if(leng==0)//leng can't equ zero
		return 0;
	memcpy(buf,src,leng);//拷贝 src 所指的内存内容的前 leng 个字节到 buf 所指的内存地址上
                         //返回指向buf的指针

	for(i=0;i<leng;i++)
	{
		if((buf[i]<'0') || (buf[i]>'f'))
			return 0;//it's no ascii
		if((buf[i]>'9') && (buf[i]<'A'))
			return 0;//it's no ascii
		if((buf[i]>'F') && (buf[i]<'a'))
			return 0;//it's no ascii
	}

	l=0;
	k=1;	//冥数
	for(i=0;i<leng;i++)
	{
		x=buf[leng-1-i]&0x0f;//取低4位
		if(buf[leng-1-i]>'9')
			x+=9;//'A~f' or 'a~f'
		l+=(x*k);

		if(mode==dec10)
			k*=10;//10进制
		else
			k*=16;//16进制
	}
	return l;
}


//***************************************************************
//ASCII字符串换成16进制码
//***************************************************************
int AscHex(__u8 *p1)
{
	int i;

	i=AscBcd(p1);
	return BcdHex(i);
}


//***************************************************************
//16进制码换成16进制显示码
//***************************************************************
void HexHexasc(int i,__u8 *p)
{
	__u8 j;

	for(j=8;j>0;j--)
	{
		p[j-1]=(i&0x0f)+0x30;
		if(p[j-1]>0x39)
			p[j-1]+=7;
		i>>=4;
	}
}


//***************************************************************
//BCD十进制码换成16进制码
//***************************************************************
int BcdHex(int i)
{
	int j,k;

	j=i&0x0f;
	i>>=4;
	k=(i&0x0f)*10;
	i>>=4;
	j+=k;
	k=(i&0x0f)*100;
	i>>=4;
	j+=k;
	k=(i&0x0f)*1000;
	i>>=4;
	j+=k;
	k=(i&0x0f)*10000;
	i>>=4;
	j+=k;
	k=(i&0x0f)*100000;
	i>>=4;
	j+=k;
	k=(i&0x0f)*1000000;
	i>>=4;
	j+=k;
	k=(i&0x0f)*10000000;
	i>>=4;
	j+=k;
	return j;
}


//***************************************************************
//ASCII字符串换成十进制BCD码
//***************************************************************
int AscBcd(__u8 *p1)
{
	__u8 j;
	int i;

	for(j=0;j<8;j++)
	{
		i<<=4;
		i|=(p1[j]&0x0f);
	}
	return i;
}


//***************************************************************
//16进制码换成十进制BCD码
//***************************************************************
int HexBcd(int i)
{
	int j;

	while(i!=0)
	{
		j<<=4;
        j|= (i%10);
	    i=i/10;
	}
	return j;
}


//***************************************************************
//16进制码换成ASCII码(0~99999999)
//***************************************************************
void HexAsc(int i,__u8 *p)
{
	__u8 L;

	if(i>99999999)
		i=99999999;
	for(L=0;L<7;L++)
	{
		*p=0x30;
		p++;
	}
	*p=0x30;			//先填7个'0'
	while(i!=0)
	 {
        *p = (i%10) + 0x30;
	    i=i/10;
		p--;
     }
}


//**********************************************************************
//映射汉字库点阵
//**********************************************************************
int DotInit(void)
{
	struct stat x;

//hzk32：32*32汉字库
	//1.打开文件
	handle_hzk32 = open(hzk32, O_RDONLY);// hzk32[]={"./font/HZK32"}; 32点阵汉字库
	if (!handle_hzk32)
    {
        printf("Error: cannot open file hzk32.\n");
        return(1);//不能打开32点阵汉字库
    }
	//2.由文件描述词取得文件状态
	fstat(handle_hzk32,&x);//将参数 handle_hzk32 所指的文件状态,复制到参数x所指的结构中

	//3.把handle_hzk32所指的文件从开始到x.st_size大小的内容映射出来，得到这块空间的指针
    phzk32 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_hzk32, 0);//建立与内存的映射
    if (phzk32 == MAP_FAILED)//MAP_FAILED==-1
    {
        printf("Error: failed to map file hzk32 to memory.\n");//建立与内存的映射失败
        return(2);
    }

    //4.字库长度x.st_size
	hzk32_size=x.st_size;
	printf("Founded file hzk32 size = %d\n",hzk32_size);


//hzk24：24*24汉字库
	handle_hzk24 = open(hzk24, O_RDONLY);
	if (!handle_hzk24)
    {
        printf("Error: cannot open file hzk24.\n");
        return(1);  			//不能打开24点阵汉字库
    }
	fstat(handle_hzk24,&x);//取文件大小
    phzk24 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_hzk24, 0);//建立与内存的映射
    if (phzk24 == MAP_FAILED)
    {
        printf("Error: failed to map file hzk24 to memory.\n");//建立与内存的映射失败
        return(2);
    }
	hzk24_size=x.st_size;
	printf("Founded file hzk24 size = %d\n",hzk24_size);


//hzk16：16*16汉字库
	handle_hzk16 = open(hzk16, O_RDONLY);
	if (!handle_hzk16)
    {
        printf("Error: cannot open file hzk16.\n");
        return(3);  			//不能打开16点阵汉字库
    }
	fstat(handle_hzk16,&x);//取文件大小
    phzk16 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_hzk16, 0);//建立与内存的映射
    if (phzk16 == MAP_FAILED)
    {
        printf("Error: failed to map file hzk16 to memory.\n");//建立与内存的映射失败
        return(4);
    }
	hzk16_size=x.st_size;
	printf("Founded file hzk16 size = %d\n",hzk16_size);


//asc32：32点阵ASC码
	handle_asc32 = open(asc32, O_RDONLY);
	if (!handle_asc32)
    {
        printf("Error: cannot open file asc32.\n");
        return(5);  			//不能打开32点阵字符库
    }
	fstat(handle_asc32,&x);//取文件大小
    pasc32 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_asc32, 0);//建立与内存的映射
    if (pasc32 == MAP_FAILED)
    {
        printf("Error: failed to map file asc32 to memory.\n");//建立与内存的映射失败
        return(6);
    }
	asc32_size=x.st_size;
	printf("Founded file asc32 size = %d\n",asc32_size);


//asc24：24点阵ASC码
	handle_asc24 = open(asc24, O_RDONLY);
	if (!handle_asc24)
    {
        printf("Error: cannot open file asc24.\n");
        return(5);  			//不能打开24点阵字符库
    }
	fstat(handle_asc24,&x);//取文件大小
    pasc24 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_asc24, 0);//建立与内存的映射
    if (pasc24 == MAP_FAILED)
    {
        printf("Error: failed to map file asc24 to memory.\n");//建立与内存的映射失败
        return(6);
    }
	asc24_size=x.st_size;
	printf("Founded file asc24 size = %d\n",asc24_size);


//asc16：16点阵ASC码
	handle_asc16 = open(asc16, O_RDONLY);
	if (!handle_asc16)
    {
        printf("Error: cannot open file asc16.\n");
        return(7);  			//不能打开16点阵字符库
    }
	fstat(handle_asc16,&x);//取文件大小
    pasc16 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_asc16, 0);//建立与内存的映射
    if (pasc16 == MAP_FAILED)
    {
        printf("Error: failed to map file asc16 to memory.\n");//建立与内存的映射失败
        return(8);
    }
	asc16_size=x.st_size;
	printf("Founded file asc16 size = %d\n",asc16_size);

    return 0;
}


//**********************************************************************
//TFT清屏
//系统的背景色:background==colour
//**********************************************************************
void FbClear(__u16 colour)
{
	int x = 0, y = 0;

    for(y = 0; y < vinfo.yres;  y++)//帧缓冲器硬件信息vinfo
    {
        for(x = 0; x < vinfo.xres ; x++)
        {
            *(fbp565 + y * vinfo.xres + x) = colour;//fbp565为显存缓冲器地址
        }
	}
}


//**********************************************************************
//TFT设备测试
#define RED_COLOR565 0XF100
#define GREEN_COLOR565 0X07E0
#define BLUE_COLOR565 0X001F
//**********************************************************************
void FbTest(void)
{
	int x = 0, y = 0;

	if(vinfo.bits_per_pixel == 16)
    {
        printf("16 bpp framebuffer\n");

        // Red Screen
        printf("Red Screen\n");
        for(y = 0; y < vinfo.yres/3;  y++)
        {
            for(x = 0; x < vinfo.xres ; x++)
            {
                *(fbp565 + y * vinfo.xres + x) = RED_COLOR565;
            }
        }

        // Green Screen
        printf("Green Screen\n");
        for(y = vinfo.yres/3; y < (vinfo.yres*2)/3; y++)
        {
            for(x = 0; x < vinfo.xres; x++)
            {
                *(fbp565 + y * vinfo.xres + x) =GREEN_COLOR565;
            }
        }

        // Blue Screen
        printf("Blue Screen\n");
        for(y = (vinfo.yres*2)/3; y < vinfo.yres; y++)
        {
            for(x = 0; x < vinfo.xres; x++)
            {
                *(fbp565 + y * vinfo.xres + x) = BLUE_COLOR565;
            }
        }
    }
	else if(vinfo.bits_per_pixel == 32)
    {
        printf("24 bpp framebuffer\n");

        // Red Screen
        printf("Red Screen\n");
        for(y = 0; y < vinfo.yres/3;  y++)
        {
            for(x = 0; x < vinfo.xres ; x++)
            {
                *(fbp565 + y * vinfo.xres + x) = RED_COLOR565;
            }
        }

        // Green Screen
        printf("Green Screen\n");
        for(y = vinfo.yres/3; y < (vinfo.yres*2)/3; y++)
        {
            for(x = 0; x < vinfo.xres; x++)
            {
                *(fbp565 + y * vinfo.xres + x) =GREEN_COLOR565;
            }
        }

        // Blue Screen
        printf("Blue Screen\n");
        for(y = (vinfo.yres*2)/3; y < vinfo.yres; y++)
        {
            for(x = 0; x < vinfo.xres; x++)
            {
                *(fbp565 + y * vinfo.xres + x) = BLUE_COLOR565;
            }
        }
    }

    else
    {
        printf("warnning: bpp is not 16 or 24\n");
    }
 }


//**********************************************************************
//TFT屏硬件内存映射
//return==0 操作成功 非0不成功
//在结构finfo有返回硬件信息
//在结构vinfo有返回TFT屏信息
//screen_SIZE==freamebuffer legnth
//fb_fb==句柄
//fbp565==内存首址
//**********************************************************************
int FbInit(void)
{
	/*1.打开帧缓冲设备*/
	fd_fb = open(tftdevice, O_RDWR);//tftdevice[]={"/dev/fb0"};
	if (!fd_fb)
    {
        printf("Error: cannot open framebuffer device.\n");
        return(1);//不能打开帧缓冲器设备
    }
	printf("Open framebuffer device </dev/fb0> successfuly! \n");

	/*2.获取帧缓冲器硬件信息（固定的屏幕参数）*/
    if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information.\n");
        return(2);//不能获取帧缓冲器硬件信息
    }

    /*3.获取屏幕可变参数*/
    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information.\n");
        return(3);//不能获取屏幕信息
    }

    /*4.计算一帧图像的大小（显示缓冲区长度）*/
    screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;//缓冲区字节大小
	printf("%dx%d, %dbpp, screen_size = %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screen_size );

	/* 5.将屏幕缓冲区映射到用户空间
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
	fbp565 = (short *)mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);//建立与内存的映射
    if ((int)fbp565 == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");//建立与内存的映射失败
        return(4);
    }
    return 0;
}


//**********************************************************************
//关闭句柄与映射的内存
//**********************************************************************
void FClose(void)
{
	/*1.释放显存的映射*/
	munmap(fbp565, screen_size);//释放显存的映射

	/*2.关闭帧缓冲设备*/
	close(fd_fb);

	/*3.释放字库的映射内存*/
	munmap(phzk32, hzk32_size);//释放字库的映射内存
	close(handle_hzk32);//关闭字库文件的句柄
	munmap(phzk24, hzk24_size);
	close(handle_hzk24);
	munmap(phzk16, hzk16_size);
	close(handle_hzk16);
	munmap(pasc32, asc32_size);
	close(handle_asc32);
	munmap(pasc24, asc24_size);
	close(handle_asc24);
	munmap(pasc16, asc16_size);
	close(handle_asc16);
}
//**********************************************************************


