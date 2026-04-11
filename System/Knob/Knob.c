#include "Knob.h"

#include "main.h"
#include "adc.h"

#include <stdint.h>

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
            Knob_DeadZoneProcess(&ADC_Buffer[i], &ADC_BufferProcessed[i], CAPTURE_LOWER_LIMIT, CAPTURE_UPPER_LIMIT);
        }
    }
}