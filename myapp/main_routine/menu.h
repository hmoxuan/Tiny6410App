/**
 * menu.h file implement Tiny6410.
 * Author: qinfei 2015.03.04
 */
#ifndef __MEHU_H__
#define __MEHU_H__

#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif

/*menu��ʼ��:�Բ˵����б�Ҫ��ʼ��*/
void menu_init(void);

/*��������ѡ��˵���Ӧ����*/
//void menu_cmd(void);
void menu_cmd(char **argv);//argv[1]����LCD��ʾ��ͼƬ

#ifdef __cplusplus
}
#endif

#endif	/*__MEHU_H__*/

