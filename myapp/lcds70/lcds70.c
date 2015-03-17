/**
 * lcds70.c file implement Tiny6410.
 * Author: qinfei 2015.03.11
 * 
 * ���Է�����
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
#include <types.h>			//��������
#include "resource.h"		//��Դ�ļ�
#include "database.h"		//���ݿ�
#include "lcds70.h"			//��������

//**********************************************************************
//������
//**********************************************************************
//int lcds70_AppCtl(char **argv)
int lcds70_AppCtl(void)
{
	int i;
	printf("Start to test LCD......\n");

	/*1.TFT��Ӳ���ڴ�ӳ��*/
	if(FbInit())//TFT����
	{
		return(1);
	}
	printf("Init the LCD !\n");
	sleep(2);

	/*2.ӳ�人�ֿ����*/
	if(DotInit())//���ֿ����
	{
		return(2);
	}
	printf("background value = black \n");
	sleep(2);

	/*3.��ָ����ɫ����*/
	FbClear(0);
	printf("Clear LCD Screen!\n");
	sleep(2);

    /*4.��ʾͼ�Σ�sw==0��ʾԭͼ,������ʾѡ��ʱ��ͼ��*/
	Dispicture(0,0,mainbmp,0);
	printf("Display main.bmp !\n");
	sleep(2);

	Dispicture(800-300,480-200,mainbmp,1);
	printf("Display bootlogo.bmp !\n");
	sleep(2);

	/*5.��ʾ�ı�����ַ���*/
	for(i=0;i<5;i++)
		DispString(mainmenu[i]);
	printf("Display menu!\n");

    /*6.�ر�TFT��������ڴ�ӳ��*/
    FClose();			//�رվ����ӳ����ڴ�

	return(0);
}


//-------------------------------------------------------------------
//��ʾͼ��
//sw==0��ʾԭͼ ������ʾѡ��ʱ��ͼ��
//x:x����ʼ���ꣻy:y����ʼ���ꡣ
//*p��ͼƬ�ļ���ָ��
//-------------------------------------------------------------------
void Dispicture(int x,int y,const char *p,int sw)
{
	int i,k,form;

	pic.x=x;//x�Ŀ�ʼ����
	pic.y=y;//y�Ŀ�ʼ����

	/*1.�����ַ���ת��������*/
	form=StrInt(p,1,dec10);

	/*2.��ȡͼƬ��*/
	memset(pic.name,0,100);//�ڴ�ռ��ʼ��Ϊ0
	for(i=2,k=0;p[i]!=',';i++,k++)
		pic.name[k]=p[i];//ԭͼ�ļ���

	/*3.��ȡѡ��ʱ�ļ���*/
	if(form)
	{
		memset(pic.names,0,100);//�ڴ�ռ��ʼ��Ϊ0
		i++;//����','
		for(k=0;p[i]!=0;i++,k++)
			pic.names[k]=p[i];//ѡ��ʱ�ļ���
	}

	printf("%d-%d-%d\n",pic.x,pic.y,form);
	printf("%s\n",pic.name);
	printf("%s\n",pic.names);

	/*4.��ʾbmpͼ��*/
	DispBmp(sw);
}


//-------------------------------------------------------------------
//��ʾbmpͼ��
//sw==0��ʾԭͼ,������ʾѡ��ʱ��ͼ��
//-------------------------------------------------------------------
int DispBmp(int sw)
{
	struct stat x;
	int handle;
	char *paddr;
	int offset,resx,resy,h,v,n,k;
	__u16 m;

//hzk32
	/*1.��ͼƬ�ļ�*/
	if(sw==0)
	{
		//��ͼƬ�ļ�
		handle = open(pic.name, O_RDONLY);
		if (!handle)
		{
			printf("Error: cannot open file %s.\n",pic.name);
			return(1);//���ܴ�1
		}
	}
	else
	{
		//��ȡѡ��ʱ���ļ���
		handle = open(pic.names, O_RDONLY);
		if (!handle)
		{
			printf("Error: cannot open file %s.\n",pic.names);
			return(2);//���ܴ�2
		}
	}

	/*2.��ȡͼƬ�ļ���С*/
	fstat(handle,&x);//ȡ�ļ���С

	/*3.�������ڴ��ӳ��*/
	paddr = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle, 0);//�������ڴ��ӳ��
    if (paddr == MAP_FAILED) //�������ڴ��ӳ��ʧ��
    {
		if(sw==0)
			printf("Error: failed to map %s to memory.\n",pic.name);
		else
			printf("Error: failed to map %s to memory.\n",pic.names);
        return(3);
    }

    /*4.����ͼƬ��С*/
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

	 /*5.��ʾͼƬ*/
	for(v=0;v<resy;v++)
	{
		n = offset + v * resx * 2;	//ÿһ�е�bmp��ַ
		k = pic.y + resy - 1 - v;
		if((k>=0) && (k<vinfo.yres))//���ڿ���������
		{
			k *= vinfo.xres;//ÿһ�е���Ļ��ַ
			for(h=0;h<resx;h++)
			{
				m=*(paddr+n+h+h+1);
				m<<=8;
				m|=*(paddr+n+h+h+0);//һ����
				if((h+pic.x) < vinfo.xres)	//���ڿ���������
					*(fbp565+k+h+pic.x)=m;
			}
		}
	}

	/*6.�ͷ��ڴ桢�ر��ļ�bmp*/
	munmap(paddr,x.st_size);//�ͷ��ڴ�
	close(handle);//�ر�bmp�ļ�
	return 0;
}


//-------------------------------------------------------------------
//��ʾ�ı�����ַ���
//��ʽ����,��,ǰ��ɫ,����ɫ,ѡ��ǰ��ɫ,ѡ�б���ɫ,�����С,�ַ���,0x00����
//-------------------------------------------------------------------
void DispString(const char *p)
{
	int c1;

	/*ȡ�ı���H��V��ǰ��ɫ������ɫ������ */
	GetHVColor(p);//ȡ�ı���H��V��ǰ��ɫ������ɫ������
	p+=33;//�ı�����ַ

	while(1)
	{
		c1=*p;
		p++;//1.ָ����һ���ı�
		if(c1 == 0)//������
			return;

		if(c1<0xa1)//2.��ASCII
		{
			if(c1>0x7f)//ä��
				c1=' ';//��space��ʾ
		}
		else//3.�����Ǻ���
		{
			if(*p<0xa1)//�ڶ�����ASCII
				c1=' ';//�鲻�ɺ�����ֻ�ܰ�space��ʾ
			else
			{
				c1<<=8;
				c1|=(*p);
				p++;//һ�����ֶ���ASCIIλ��
			}
		}

		DispDot(c1);//4.��ʾ����
	}
}


//-------------------------------------------------------------------
//��ʾ����
//i��8λΪ0����ASCII�������Ǻ��֡�
//-------------------------------------------------------------------
void DispDot(__u16 i)
{
	int h,v,leng,w,mask;//�г����г���ռ���ֽ���,��ռ���ֽ�,����
	char *addr;
	int tftaddr;
	unsigned int x,m,n;

	if(tft.font==dot16)//16����
	{
		if(i&0xff00)
		{
			h=v=16;//����
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
	else//24������ĵ���
	{
		if(tft.font==dot32)//32����
		{
			if(i&0xff00)
			{
				h=v=32;//����
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
		else//24����(Ĭ��)
		{
			if(i&0xff00)
			{
				h=v=24;//����
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
	addr+=(i*leng);//��Ӧ�ĵ�ַ
	tftaddr = tft.y*vinfo.xres + tft.x;//��Ļλ��

	x=0;//��0�п�ʼ
	while(x<v)
	{
		n=0;
		for(m=0;m<w;m++)
		{
			n<<=8;
			n|=*(addr+m);//�ϲ�ÿһ�еĵ���(���32���)
		}
		addr+=w;//ָ����һ��
		for(m=0;m<h;m++)//��0�п�ʼ(ÿ�е���)
		{
			if(n&mask)
				*(fbp565+tftaddr+m)=tft.f;
			else
				*(fbp565+tftaddr+m)=tft.b;
			n<<=1;//ָ����һ��
		}
		tftaddr+=vinfo.xres;//��Ļָ����һ��
		x++;//�м���+1
	}
	tft.x+=h;//�м�
	if(tft.x>=vinfo.xres)
	{
		tft.x=0;
		tft.y+=v;//�м�
		if(tft.y>=vinfo.yres)
			tft.y=0;
	}
}


//-------------------------------------------------------------------
//ȡ�ı���H��V��ǰ��ɫ������ɫ,����
//-------------------------------------------------------------------
void GetHVColor(const void *p)
{
	tft.x=StrInt(p,4,dec10);		//��
	tft.y=StrInt((p+5),4,dec10);	//��
	tft.f=StrInt((p+10),4,hex16);	//ǰ��ɫ
	tft.b=StrInt((p+15),4,hex16);	//����ɫ
	tft.sf=StrInt((p+20),4,hex16);	//ѡ��ʱ��ǰ��ɫ
	tft.sb=StrInt((p+25),4,hex16);	//ѡ��ʱ�ı���ɫ
	tft.font=StrInt((p+30),2,dec10);//����
}


//-------------------------------------------------------------------
//�����ַ���ת����������leng=�ַ�������
//-------------------------------------------------------------------
int StrInt(const void *src,char leng,int mode)
{
	int i,k,l,x;
	char buf[leng];

	if(leng==0)//leng can't equ zero
		return 0;
	memcpy(buf,src,leng);//���� src ��ָ���ڴ����ݵ�ǰ leng ���ֽڵ� buf ��ָ���ڴ��ַ��
                         //����ָ��buf��ָ��

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
	k=1;	//ڤ��
	for(i=0;i<leng;i++)
	{
		x=buf[leng-1-i]&0x0f;//ȡ��4λ
		if(buf[leng-1-i]>'9')
			x+=9;//'A~f' or 'a~f'
		l+=(x*k);

		if(mode==dec10)
			k*=10;//10����
		else
			k*=16;//16����
	}
	return l;
}


//***************************************************************
//ASCII�ַ�������16������
//***************************************************************
int AscHex(__u8 *p1)
{
	int i;

	i=AscBcd(p1);
	return BcdHex(i);
}


//***************************************************************
//16�����뻻��16������ʾ��
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
//BCDʮ�����뻻��16������
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
//ASCII�ַ�������ʮ����BCD��
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
//16�����뻻��ʮ����BCD��
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
//16�����뻻��ASCII��(0~99999999)
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
	*p=0x30;			//����7��'0'
	while(i!=0)
	 {
        *p = (i%10) + 0x30;
	    i=i/10;
		p--;
     }
}


//**********************************************************************
//ӳ�人�ֿ����
//**********************************************************************
int DotInit(void)
{
	struct stat x;

//hzk32��32*32���ֿ�
	//1.���ļ�
	handle_hzk32 = open(hzk32, O_RDONLY);// hzk32[]={"./font/HZK32"}; 32�����ֿ�
	if (!handle_hzk32)
    {
        printf("Error: cannot open file hzk32.\n");
        return(1);//���ܴ�32�����ֿ�
    }
	//2.���ļ�������ȡ���ļ�״̬
	fstat(handle_hzk32,&x);//������ handle_hzk32 ��ָ���ļ�״̬,���Ƶ�����x��ָ�Ľṹ��

	//3.��handle_hzk32��ָ���ļ��ӿ�ʼ��x.st_size��С������ӳ��������õ����ռ��ָ��
    phzk32 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_hzk32, 0);//�������ڴ��ӳ��
    if (phzk32 == MAP_FAILED)//MAP_FAILED==-1
    {
        printf("Error: failed to map file hzk32 to memory.\n");//�������ڴ��ӳ��ʧ��
        return(2);
    }

    //4.�ֿⳤ��x.st_size
	hzk32_size=x.st_size;
	printf("Founded file hzk32 size = %d\n",hzk32_size);


//hzk24��24*24���ֿ�
	handle_hzk24 = open(hzk24, O_RDONLY);
	if (!handle_hzk24)
    {
        printf("Error: cannot open file hzk24.\n");
        return(1);  			//���ܴ�24�����ֿ�
    }
	fstat(handle_hzk24,&x);//ȡ�ļ���С
    phzk24 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_hzk24, 0);//�������ڴ��ӳ��
    if (phzk24 == MAP_FAILED)
    {
        printf("Error: failed to map file hzk24 to memory.\n");//�������ڴ��ӳ��ʧ��
        return(2);
    }
	hzk24_size=x.st_size;
	printf("Founded file hzk24 size = %d\n",hzk24_size);


//hzk16��16*16���ֿ�
	handle_hzk16 = open(hzk16, O_RDONLY);
	if (!handle_hzk16)
    {
        printf("Error: cannot open file hzk16.\n");
        return(3);  			//���ܴ�16�����ֿ�
    }
	fstat(handle_hzk16,&x);//ȡ�ļ���С
    phzk16 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_hzk16, 0);//�������ڴ��ӳ��
    if (phzk16 == MAP_FAILED)
    {
        printf("Error: failed to map file hzk16 to memory.\n");//�������ڴ��ӳ��ʧ��
        return(4);
    }
	hzk16_size=x.st_size;
	printf("Founded file hzk16 size = %d\n",hzk16_size);


//asc32��32����ASC��
	handle_asc32 = open(asc32, O_RDONLY);
	if (!handle_asc32)
    {
        printf("Error: cannot open file asc32.\n");
        return(5);  			//���ܴ�32�����ַ���
    }
	fstat(handle_asc32,&x);//ȡ�ļ���С
    pasc32 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_asc32, 0);//�������ڴ��ӳ��
    if (pasc32 == MAP_FAILED)
    {
        printf("Error: failed to map file asc32 to memory.\n");//�������ڴ��ӳ��ʧ��
        return(6);
    }
	asc32_size=x.st_size;
	printf("Founded file asc32 size = %d\n",asc32_size);


//asc24��24����ASC��
	handle_asc24 = open(asc24, O_RDONLY);
	if (!handle_asc24)
    {
        printf("Error: cannot open file asc24.\n");
        return(5);  			//���ܴ�24�����ַ���
    }
	fstat(handle_asc24,&x);//ȡ�ļ���С
    pasc24 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_asc24, 0);//�������ڴ��ӳ��
    if (pasc24 == MAP_FAILED)
    {
        printf("Error: failed to map file asc24 to memory.\n");//�������ڴ��ӳ��ʧ��
        return(6);
    }
	asc24_size=x.st_size;
	printf("Founded file asc24 size = %d\n",asc24_size);


//asc16��16����ASC��
	handle_asc16 = open(asc16, O_RDONLY);
	if (!handle_asc16)
    {
        printf("Error: cannot open file asc16.\n");
        return(7);  			//���ܴ�16�����ַ���
    }
	fstat(handle_asc16,&x);//ȡ�ļ���С
    pasc16 = mmap(0, x.st_size, PROT_READ, MAP_SHARED, handle_asc16, 0);//�������ڴ��ӳ��
    if (pasc16 == MAP_FAILED)
    {
        printf("Error: failed to map file asc16 to memory.\n");//�������ڴ��ӳ��ʧ��
        return(8);
    }
	asc16_size=x.st_size;
	printf("Founded file asc16 size = %d\n",asc16_size);

    return 0;
}


//**********************************************************************
//TFT����
//ϵͳ�ı���ɫ:background==colour
//**********************************************************************
void FbClear(__u16 colour)
{
	int x = 0, y = 0;

    for(y = 0; y < vinfo.yres;  y++)//֡������Ӳ����Ϣvinfo
    {
        for(x = 0; x < vinfo.xres ; x++)
        {
            *(fbp565 + y * vinfo.xres + x) = colour;//fbp565Ϊ�Դ滺������ַ
        }
	}
}


//**********************************************************************
//TFT�豸����
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
//TFT��Ӳ���ڴ�ӳ��
//return==0 �����ɹ� ��0���ɹ�
//�ڽṹfinfo�з���Ӳ����Ϣ
//�ڽṹvinfo�з���TFT����Ϣ
//screen_SIZE==freamebuffer legnth
//fb_fb==���
//fbp565==�ڴ���ַ
//**********************************************************************
int FbInit(void)
{
	/*1.��֡�����豸*/
	fd_fb = open(tftdevice, O_RDWR);//tftdevice[]={"/dev/fb0"};
	if (!fd_fb)
    {
        printf("Error: cannot open framebuffer device.\n");
        return(1);//���ܴ�֡�������豸
    }
	printf("Open framebuffer device </dev/fb0> successfuly! \n");

	/*2.��ȡ֡������Ӳ����Ϣ���̶�����Ļ������*/
    if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information.\n");
        return(2);//���ܻ�ȡ֡������Ӳ����Ϣ
    }

    /*3.��ȡ��Ļ�ɱ����*/
    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information.\n");
        return(3);//���ܻ�ȡ��Ļ��Ϣ
    }

    /*4.����һ֡ͼ��Ĵ�С����ʾ���������ȣ�*/
    screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;//�������ֽڴ�С
	printf("%dx%d, %dbpp, screen_size = %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screen_size );

	/* 5.����Ļ������ӳ�䵽�û��ռ�
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
	fbp565 = (short *)mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);//�������ڴ��ӳ��
    if ((int)fbp565 == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");//�������ڴ��ӳ��ʧ��
        return(4);
    }
    return 0;
}


//**********************************************************************
//�رվ����ӳ����ڴ�
//**********************************************************************
void FClose(void)
{
	/*1.�ͷ��Դ��ӳ��*/
	munmap(fbp565, screen_size);//�ͷ��Դ��ӳ��

	/*2.�ر�֡�����豸*/
	close(fd_fb);

	/*3.�ͷ��ֿ��ӳ���ڴ�*/
	munmap(phzk32, hzk32_size);//�ͷ��ֿ��ӳ���ڴ�
	close(handle_hzk32);//�ر��ֿ��ļ��ľ��
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


