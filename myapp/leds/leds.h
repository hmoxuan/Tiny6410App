/**
 * leds.c file implement Tiny6410.
 * Author: qinfei
 */
#ifndef __LEDS_H__
#define __LEDS_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif


/*leds初始化:打开设备文件、打开所有的LED*/
int Leds_Init(void);

/*leds关闭设备文件:关闭所有的LED、释放资源*/
void Leds_Destroy(void);

/*leds应用控制:实现LED具体的应用逻辑控制*/
void Leds_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__LEDS_H__*/
