#ifndef __KNOB_H__
#define __KNOB_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "../Common/Common.h"

// #include "main.h" // Moved to Common.h

// /* ---- Marcos From Knobs ----*/
// #define CAPTURE_UPPER_LIMIT 4060        // 捕获下限值
// #define CAPTURE_LOWER_LIMIT 32          // 捕获上限值
// #define ADC_CHANNEL_COUNT   4           // ADC 通道数
#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
extern uint16_t ADC_Buffer[4];
extern float ADC_BufferProcessed[4];

/* ---- RTOS ---- */
void Knobs_Update_TaskInit(void);
void Knobs_Update_SemInit(void);

extern osSemaphoreId_t Knobs_Update_SemHandle;
#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __KNOB_H__ */