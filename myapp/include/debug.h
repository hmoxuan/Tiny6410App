/*
 *My Tiny6410 App test
 *debug.h
 *Created by qinfei 20150304
 *
 *debug info
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {/*表示编译生成的内容符号名，使用C约定*/
#endif

#include <stdio.h>

/*如果想打印dbg(x...)和err(x...)的信息，就定义DEBUG*/
#define DEBUG

#ifdef DEBUG
#define dbg(x...)	printf(x)
#define err(x...)	printf(x)

#else
#define dbg(x...)	do {}while(0)
#define DEBUG_ERR	1
#define err(x...)	if(DEBUG_ERR) printf(x)
#endif

#ifdef __cplusplus
}
#endif

#endif

