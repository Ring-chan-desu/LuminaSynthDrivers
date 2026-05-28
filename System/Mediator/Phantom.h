#ifndef __PHANTOM_H__
#define __PHANTOM_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../Common/Common.h"


/* ---- 2. C/C++ 通用类型声明 ---- */
class Phantom   //  影子寄存器基类用于防止ISR内部调制导致爆音
{
    
};

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __PHANTOM_H__ */