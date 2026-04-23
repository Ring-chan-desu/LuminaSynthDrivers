#ifndef __WAVEFORM_H__
#define __WAVEFORM_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "main.h"
#include <stdint.h>
#include "../System/Interface/Lumina_Interface.h"

#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
extern const uint16_t* WaveFormList[WAVEFORM_COUNT];

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __WAVEFORM_H__ */