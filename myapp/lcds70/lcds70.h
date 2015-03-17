/**
 * lcds70.h file implement Tiny6410.
 * Author: qinfei 2015.03.11
 */
#ifndef __LCDS70_H__
#define __LCDS70_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif

/*Created By qinfei*/
#include <types.h>


int FbInit(void);								//TFT屏硬件内存映射
void FClose(void);								//关闭TFT屏句柄与内存映射
void FbClear(__u16 colour);						//用指定颜色清屏
void FbTest(void);								//TFT测试
int DotInit(void);								//映射汉字库点阵
void GetHVColor(const void *p);					//取文本的H,V,前景色,背景色,字体
void DispString(const char *p);					//显示文本混合字符串
void DispDot(__u16 i);							//显示点阵 i高8位为0 是ASCII 非则即是汉字
void DispArray(const char *p);					//显示陈列
void Dispicture(int x,int y,const char *p,int sw);//显示图形 sw==0显示原图 非则显示选中时的图形
int DispBmp(int sw);							//显示bmp图形 sw==0显示原图 非则显示选中时的图形


void HexAsc(int i,__u8 *p);						//16进制码换成ASCII码(0~99999999)
int AscBcd(__u8 *p1);							//ASCII字符串换成十进制BCD码
int BcdHex(int i);								//BCD十进制码换成16进制码
int AscHex(__u8 *p1);							//ASCII字符串换成16进制码
int HexBcd(int i);								//16进制码换成十进制BCD码
void HexHexasc(int i,__u8 *p);					//16进制码换成16进制ASCII码
int StrInt(const void *src,char leng,int mode);	//字符串转换成整数 leng=字符串长度 mode==dec10 or hex16

//int lcds70_AppCtl(char **argv);//argv[1]传递LCD显示的图片
int lcds70_AppCtl(void);

#ifdef __cplusplus
}
#endif

#endif  /*__LCDS70_H__*/






