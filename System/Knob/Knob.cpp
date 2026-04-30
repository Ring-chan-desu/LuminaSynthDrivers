#include "./Knob.h"
#include "../System/Interface/Lumina_Interface.h"

#include "main.h"
#include "adc.h"
#include "stm32f407xx.h"
#include "tim.h"

#include <stdint.h>

#include "../System/Mediator/Mediator.h"

uint16_t ADC_Buffer[ADC_CHANNEL_COUNT] = {0};
// 1. 将处理后的缓冲区改为 float 以存储权重
float ADC_BufferProcessed[ADC_CHANNEL_COUNT] = {0};

/**
 * @brief 重构后的死区处理与权重计算函数
 * @param pSource 原始 ADC 采样值指针
 * @param pValue 权重输出指针 (float)
 * @param CaptureMin 采样下限死区
 * @param CaptureMax 采样上限死区
 * 
 * 原理说明：
 * - 基础截断：若输入小于 Min，输出 0.0；若大于 Max，输出 1.0。
 * - 线性映射：处于 [Min, Max] 之间时，减去基数 Min 并除以有效量程 (Max - Min)，
 *   从而将原始数值线性归一化为 0.0 到 1.0 的权重。
 */
void Knob_DeadZoneProcess(uint16_t* pSource, float* pValue, uint16_t CaptureMin, uint16_t CaptureMax){
    if (*pSource <= CaptureMin) {
        // 低于下限死区，权重为 0
        *pValue = 0.0f;
    }
    else if (*pSource >= CaptureMax) {
        // 高于上限死区，权重为 1
        *pValue = 1.0f;
    }
    else {
        // 在有效量程内进行映射计算
        // 映射公式: (当前值 - 下限) / (上限 - 下限)
        *pValue = (float)(*pSource - CaptureMin) / (float)(CaptureMax - CaptureMin);
    }
}

/* ---- 回调部分 ---- */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
            // 2. 调用重构后的逻辑，直接输出权重到 float 数组
            Knob_DeadZoneProcess(&ADC_Buffer[i], &ADC_BufferProcessed[i], CAPTURE_LOWER_LIMIT, CAPTURE_UPPER_LIMIT);
        }
    }
}