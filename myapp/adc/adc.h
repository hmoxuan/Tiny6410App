/**
 * adc.h file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif


/*adc��ʼ��:���豸�ļ�*/
int adc_Init(void);

/*adc�ر��豸�ļ�:�ͷ���Դ*/
void adc_Destroy(void);

/*adcӦ�ÿ���:ʵ��adc�����Ӧ���߼�����*/
void adc_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__ADC_H__*/








