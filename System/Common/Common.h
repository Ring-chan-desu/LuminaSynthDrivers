/******************************************************************************
 * *
 * [ SYSTEM LOG : ORIENTATION ]                                              *
 * ────────────────────────────────────────────────────────────────────────  *
 * *
 * 致 瞬 息 萬 變 之 物 ， 即 恆 古 不 變 之 物 。                           *
 * *
 * ────────────────────────────────────────────────────────────────────────  *
 * [ TO THE CHANGING, AND THE CHANGELESS. ]                                  *
 * *
 ******************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

/* ---- 1. 通用驱动头文件 ---- */
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "cmsis_os.h"
#include "task.h"
#include "main.h"
// #include "stm32f407xx.h"
// #include "stm32f103xx.h"

/* ---- 2. 标准类型定义 ---- */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"

#ifdef __cplusplus
}
#endif

/* ---- 3. 项目共用宏定义 ---- */
#define SAMPLE_RATE             48000           // 采样率
#define WAVEFORM_LENGTH         1024            // 波表长度
#define HALF_BUFFER_LENGTH      256             // 半缓冲区长度
#define FULL_BUFFER_LENGTH      512             // 全缓冲区长度

#define CAPTURE_UPPER_LIMIT     4060            // 捕获下限值:文件内部定义
#define CAPTURE_LOWER_LIMIT     32              // 捕获上限值:文件内部定义
#define ADC_CHANNEL_COUNT       4               // ADC 通道数:文件内部定义

#define WAVEFORM_COUNT          3               // 波表个数:文件内部定义

#define ADSR_POINT_COUNT        3               //  ADSR有效点数:文件内部定义
#define ADSR_HEIGHT             100.0f          //  ADSR高度:文件内部定义
#define ADSR_WIDTH              100.0f          //  ADSR宽度:文件内部定义
#define ADSR_PERIOD             3.0f            //  ADSR周期(秒)：控制完整 ADSR 曲线循环的默认时长（秒）。默认值 3 秒，可按需调整。

#ifdef __cplusplus
    static constexpr float FM_CONSTANT      = static_cast<float>(WAVEFORM_LENGTH) / SAMPLE_RATE;                //  调频常量 能在编译阶段就被计算好
    static constexpr float ADSR_FM_CONSTANT = static_cast<float>(ADSR_WIDTH) / (SAMPLE_RATE * ADSR_PERIOD);     //  ADSR调频常量
#endif

#endif /* __COMMON_H__ */