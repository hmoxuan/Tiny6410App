/*
 *My Tiny6410 App
 *database.h
 *Created by qinfei 20150315
 */
#ifndef _DATABASE_H
#define _DATABASE_H

/*Created By qinfei*/
#include <types.h>

const __u8 tftdevice[]={"/dev/fb0"};	//�Դ�֡�������豸��
const __u8 hzk32[]={"./font/HZK32"};	//32�����ֿ�
const __u8 hzk24[]={"./font/HZK24"};	//24�����ֿ�
const __u8 hzk16[]={"./font/HZK16"};	//16��µĺ��ֿ�
const __u8 asc32[]={"./font/ASC32"};	//32��µ�asc��
const __u8 asc24[]={"./font/ASC24"};	//24��µ�asc��
const __u8 asc16[]={"./font/ASC16"};	//16��µ�asc��

/*************************************************************************
//��ʽ����,��,ǰ��ɫ,����ɫ,ѡ��ǰ��ɫ,ѡ�б���ɫ,�����С,�ַ���,0x00����
*************************************************************************/
const char mainmenu[5][46]={
/* ��----��---ǰ��ɫ--����ɫ-ѡ��ǰ��ɫ-ѡ�б���ɫ-�����С-�ַ���-0x00����                                */
/*0260,0100,ffff, 0000, f100,   0000,   32,  1-����      \0              */

{"0260,0100,ffff,0000,f100,0000,32,1 - ����\0"},
{"0260,0160,ffff,0000,f100,0000,32,2 - ��������\0"},
{"0260,0220,ffff,0000,f100,0000,32,3 - �˵�����\0"},
{"0260,0280,ffff,0000,f100,0000,32,4 - �ļ��洢\0"},
{"0260,0340,ffff,0000,f100,0000,32,5 - ���ܲ���\0"},
};
const char mainbmp[]={"1,./picture/main.bmp,./picture/mains.bmp\0"};
#endif /* _DATABASE_H */