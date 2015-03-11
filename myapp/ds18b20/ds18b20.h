/**
 * ds18b20.h file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#ifndef __DS18B20_H__
#define __DS18B20_H__

#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif


/*ds18b20��ʼ��:���豸�ļ�*/
int ds18b20_Init(void);

/*ds18b20�ر��豸�ļ�:�ͷ���Դ*/
void ds18b20_Destroy(void);

/*ds18b20Ӧ�ÿ���:ʵ��ds18b20�����Ӧ���߼�����*/
void ds18b20_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__DS18B20_H__*/
