#include "./Knob.h"
#include "../System/Interface/Lumina_Interface.h"


#include "main.h"
#include "adc.h"
#include "stm32f407xx.h"
#include "tim.h"
#include "arm_math.h"

#include <stdint.h>

#include "../System/Mediator/Mediator.h"

uint16_t ADC_Buffer[ADC_CHANNEL_COUNT] = {0};
uint16_t ADC_BufferProcessed[ADC_CHANNEL_COUNT] = {0};

void Knob_DeadZoneProcess(uint16_t* pSource, uint16_t* pValue, uint16_t CaptureMin, uint16_t CaptureMax){
    if (*pSource < CaptureMin) {
        *pValue = 0;
    }
    else if (*pSource > CaptureMax) {
        *pValue = CAPTURE_UPPER_LIMIT;
    }
    else {
        *pValue = *pSource;
    }
}

/* ---- 回调部分 ---- */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
            // 等后面安装上了 CMSIS DSP 库再改成 arm_clip_q15
            Knob_DeadZoneProcess(&ADC_Buffer[i], &ADC_BufferProcessed[i], CAPTURE_LOWER_LIMIT, CAPTURE_UPPER_LIMIT);
            // arm_clip_q15((q15_t*)ADC_Buffer, (q15_t*)ADC_BufferProcessed, CAPTURE_LOWER_LIMIT, CAPTURE_UPPER_LIMIT, ADC_CHANNEL_COUNT);
        }
    }
}