/**
 * ds18b20.h file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#ifndef __DS18B20_H__
#define __DS18B20_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif


/*ds18b20初始化:打开设备文件*/
int ds18b20_Init(void);

/*ds18b20关闭设备文件:释放资源*/
void ds18b20_Destroy(void);

/*ds18b20应用控制:实现ds18b20具体的应用逻辑控制*/
void ds18b20_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__DS18B20_H__*/
