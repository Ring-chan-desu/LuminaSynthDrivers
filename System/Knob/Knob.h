#ifndef __KNOB_H__
#define __KNOB_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "main.h"

#define CAPTURE_UPPER_LIMIT 4092      // 捕获下限值
#define CAPTURE_LOWER_LIMIT 32    // 捕获上限值

#define ADC_CHANNEL_COUNT 4 
#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
extern uint16_t ADC_Buffer[4];
extern uint16_t ADC_BufferProcessed[4];

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __KNOB_H__ */