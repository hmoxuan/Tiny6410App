/**
 * keys.h file implement Tiny6410.
 * Author: qinfei 2015.03.09
 */
#ifndef __KEYS_H__
#define __KEYS_H__

#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif


/*keys��ʼ��:���豸�ļ�*/
int keys_Init(void);

/*keys�ر��豸�ļ�:�ͷ���Դ*/
void keys_Destroy(void);

/*keysӦ�ÿ���:ʵ��keys�����Ӧ���߼�����*/
void keys_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__KEYS_H__*/





