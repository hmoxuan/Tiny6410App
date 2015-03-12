/**
 * menu.h file implement Tiny6410.
 * Author: qinfei 2015.03.04
 */
#ifndef __MEHU_H__
#define __MEHU_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif

/*menu初始化:对菜单进行必要初始化*/
void menu_init(void);

/*根据命令选择菜单相应操作*/
//void menu_cmd(void);
void menu_cmd(char **argv);//argv[1]传递LCD显示的图片

#ifdef __cplusplus
}
#endif

#endif	/*__MEHU_H__*/

