#ifndef __OUTPUT_H__
#define __OUTPUT_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../System/Common/Common.h"    //  通用头文件包含与宏定义
#include "Output.h"


/* ---- 2. C/C++ 通用类型声明 ---- */

// class OSC;
// class ADSR;
// class LFO;
// class ParamMediator;

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */

/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
extern uint16_t    OutBuffer_q15[FULL_BUFFER_LENGTH];
extern float       OutBuffer_f32[FULL_BUFFER_LENGTH];

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __OUTPUT_H__ */