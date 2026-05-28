#ifndef __LUMINA_INTERFACE_H__
#define __LUMINA_INTERFACE_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../Common/Common.h"
// #include "main.h" // Moved to Common.h
// #include "../WaveForm/WaveForm.h" // Redundant: only common macros are used here (moved to Common.h)

/* ---- 2. C/C++ 通用类型声明 ---- */
// extern OSC

#ifdef __cplusplus
/* ---- 2.1 前置声明 ---- */
class OSC;
class ADSR;
class LFO;
class Mediator;

/* ---- 3. 仅 C++ 可见的类定义 ---- */
extern OSC OSC1;
extern LFO LFO1;
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
void Lumina_Interface_Init(void);

float* Lumina_Interface_GetOutBuffer(void);

float Lumina_Interface_ADSR_GetValue(float CurrentX);  //  测试函数

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __LUMINA_INTERFACE_H__ */