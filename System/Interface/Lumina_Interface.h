#ifndef __LUMINA_INTERFACE_H__
#define __LUMINA_INTERFACE_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "main.h"

/* ---- Marcos From OSC ----*/
#define SAMPLE_RATE             48000           // 采样率
#define WAVEFORM_LENGTH         1024            // 波表长度
#define HALF_BUFFER_LENGTH      256             // 半缓冲区长度
#define FULL_BUFFER_LENGTH      512             // 全缓冲区长度
#define FM_CONSTANT             0.021333333f    // 调频常量 (1024 / 48000)

/* ---- Marcos From Knobs ----*/
#define CAPTURE_UPPER_LIMIT 4092        // 捕获下限值
#define CAPTURE_LOWER_LIMIT 32          // 捕获上限值
#define ADC_CHANNEL_COUNT   4           // ADC 通道数

/* ---- Marcos From WaveForms ----*/    
#define WAVEFORM_COUNT  3    // 波表个数

/* ---- 2. C/C++ 通用类型声明 ---- */


#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
void Lumina_Interface_Init(void);

uint32_t* Lumina_Interface_GetOutBuffer(void);
#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __LUMINA_INTERFACE_H__ */