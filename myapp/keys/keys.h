/**
 * keys.h file implement Tiny6410.
 * Author: qinfei 2015.03.09
 */
#ifndef __KEYS_H__
#define __KEYS_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif


/*keys初始化:打开设备文件*/
int keys_Init(void);

/*keys关闭设备文件:释放资源*/
void keys_Destroy(void);

/*keys应用控制:实现keys具体的应用逻辑控制*/
void keys_AppCtl(void);


#ifdef __cplusplus
}
#endif

#endif  /*__KEYS_H__*/





