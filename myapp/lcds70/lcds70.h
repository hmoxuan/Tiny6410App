/**
 * lcds70.h file implement Tiny6410.
 * Author: qinfei 2015.03.11
 */
#ifndef __LCDS70_H__
#define __LCDS70_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif


/*lcds70应用控制:实现lcds70具体的应用逻辑控制*/
//void lcds70_AppCtl(void);
void lcds70_AppCtl(char **argv);//argv[1]传递LCD显示的图片

#ifdef __cplusplus
}
#endif

#endif  /*__LCDS70_H__*/






