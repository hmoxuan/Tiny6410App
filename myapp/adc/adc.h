/**
 * adc.h file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif


/*adc初始化:打开设备文件*/
int adc_Init(void);

/*adc关闭设备文件:释放资源*/
void adc_Destroy(void);

/*adc应用控制:实现adc具体的应用逻辑控制*/
void adc_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__ADC_H__*/








