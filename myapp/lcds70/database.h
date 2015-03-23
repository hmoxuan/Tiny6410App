/*
 *My Tiny6410 App
 *database.h
 *Created by qinfei 20150315
 */
#ifndef _DATABASE_H
#define _DATABASE_H

/*Created By qinfei*/
#include <types.h>

const __u8 tftdevice[]={"/dev/fb0"};	//显存帧缓冲器设备名
const __u8 hzk32[]={"./font/HZK32"};	//32点阵汉字库
const __u8 hzk24[]={"./font/HZK24"};	//24点阵汉字库
const __u8 hzk16[]={"./font/HZK16"};	//16点陈的汉字库
const __u8 asc32[]={"./font/ASC32"};	//32点陈的asc码
const __u8 asc24[]={"./font/ASC24"};	//24点陈的asc码
const __u8 asc16[]={"./font/ASC16"};	//16点陈的asc码

/*************************************************************************
//格式：行,列,前景色,背景色,选中前景色,选中背景色,字体大小,字符串,0x00结束
*************************************************************************/
const char mainmenu[5][46]={
/* 行----列---前景色--背景色-选中前景色-选中背景色-字体大小-字符串-0x00结束                                */
/*0260,0100,ffff, 0000, f100,   0000,   32,  1-运行      \0              */

{"0260,0100,ffff,0000,f100,0000,32,1 - 运行\0"},
{"0260,0160,ffff,0000,f100,0000,32,2 - 参数设置\0"},
{"0260,0220,ffff,0000,f100,0000,32,3 - 菜单管理\0"},
{"0260,0280,ffff,0000,f100,0000,32,4 - 文件存储\0"},
{"0260,0340,ffff,0000,f100,0000,32,5 - 功能测试\0"},
};
const char mainbmp[]={"1,./picture/main.bmp,./picture/mains.bmp\0"};
#endif /* _DATABASE_H */
