/**
 * lcds70.h file implement Tiny6410.
 * Author: qinfei 2015.03.11
 */
#ifndef __LCDS70_H__
#define __LCDS70_H__

#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif

/*Created By qinfei*/
#include <types.h>


int FbInit(void);								//TFT��Ӳ���ڴ�ӳ��
void FClose(void);								//�ر�TFT��������ڴ�ӳ��
void FbClear(__u16 colour);						//��ָ����ɫ����
void FbTest(void);								//TFT����
int DotInit(void);								//ӳ�人�ֿ����
void GetHVColor(const void *p);					//ȡ�ı���H,V,ǰ��ɫ,����ɫ,����
void DispString(const char *p);					//��ʾ�ı�����ַ���
void DispDot(__u16 i);							//��ʾ���� i��8λΪ0 ��ASCII �����Ǻ���
void DispArray(const char *p);					//��ʾ����
void Dispicture(int x,int y,const char *p,int sw);//��ʾͼ�� sw==0��ʾԭͼ ������ʾѡ��ʱ��ͼ��
int DispBmp(int sw);							//��ʾbmpͼ�� sw==0��ʾԭͼ ������ʾѡ��ʱ��ͼ��


void HexAsc(int i,__u8 *p);						//16�����뻻��ASCII��(0~99999999)
int AscBcd(__u8 *p1);							//ASCII�ַ�������ʮ����BCD��
int BcdHex(int i);								//BCDʮ�����뻻��16������
int AscHex(__u8 *p1);							//ASCII�ַ�������16������
int HexBcd(int i);								//16�����뻻��ʮ����BCD��
void HexHexasc(int i,__u8 *p);					//16�����뻻��16����ASCII��
int StrInt(const void *src,char leng,int mode);	//�ַ���ת�������� leng=�ַ������� mode==dec10 or hex16

//int lcds70_AppCtl(char **argv);//argv[1]����LCD��ʾ��ͼƬ
int lcds70_AppCtl(void);

#ifdef __cplusplus
}
#endif

#endif  /*__LCDS70_H__*/






