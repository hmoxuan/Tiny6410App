/**
 * leds.h file implement Tiny6410.
 * Author: qinfei
 */
#ifndef __LEDS_H__
#define __LEDS_H__

#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif


/*leds��ʼ��:���豸�ļ��������е�LED*/
int Leds_Init(void);

/*leds�ر��豸�ļ�:�ر����е�LED���ͷ���Դ*/
void Leds_Destroy(void);

/*ledsӦ�ÿ���:ʵ��LED�����Ӧ���߼�����*/
void Leds_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__LEDS_H__*/


