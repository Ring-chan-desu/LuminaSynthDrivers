#ifndef __RTOSBRIDGE_H__
#define __RTOSBRIDGE_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "main.h"


/* ---- 2. C/C++ 通用类型声明 ---- */


#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
void Bridge_Init(void);

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __RTOSBRIDGE_H__ */