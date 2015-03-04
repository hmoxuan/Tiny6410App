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
extern "C" {/*��ʾ�������ɵ����ݷ�������ʹ��CԼ��*/
#endif

#include <stdio.h>

/*������ӡdbg(x...)��err(x...)����Ϣ���Ͷ���DEBUG*/
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

