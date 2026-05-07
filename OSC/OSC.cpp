#include "OSC.h"

#include "main.h"
#include "tim.h"
#include "stm32f407xx.h"

#include <cstdint>
#include <stdint.h>
#include <math.h>

#include "./OSC.h"
#include "../System/Interface/Lumina_Interface.h"
#include "../System/Mediator/Mediator.h"
#include "../WaveForm/WaveForm.h"
#include "../System/Knob/Knob.h"
#include "stm32f4xx_hal_adc.h"

/* ---- 振荡器 ---- */

void OSC::OSC_StepCalculate(){
    this->Step = this->ActualFreq * FM_CONSTANT;
    this->OSC_FM(CAPTURE_UPPER_LIMIT);
}

void OSC::OSC_BufferFill(uint8_t HalfFlag){
    // 明确指定起始位置
    uint16_t* Start = (HalfFlag == 0) ? &this->Buffer[0] : &this->Buffer[HALF_BUFFER_LENGTH];
    
    for(int i = 0 ; i < HALF_BUFFER_LENGTH ; i ++){
        // Start[i] = this->OSC_Lerp() * this->OSC_AM(CAPTURE_UPPER_LIMIT);
        Start[i] = this->OSC_Lerp() * this->AM_Coeff;
        this->OSC_Accmulate();
    }
    
    if (HalfFlag) {
        this->OSC_StepCalculate();
        // this->OSC_WaveFormSelect(__HAL_TIM_GET_COUNTER(&htim3));    //  编码器给ban了,暂时注释掉
    }
}

void OSC::OSC_Accmulate(void){
    this->Accmulation += this->Step;
    if (this->Accmulation >= (float)WAVEFORM_LENGTH) {
        this->Accmulation -= (float)WAVEFORM_LENGTH;
    }
}

uint16_t OSC::OSC_Lerp(){
    uint16_t index_l = (uint16_t)this->Accmulation;
    
    uint16_t index_r = index_l + 1;
    if (index_r >= WAVEFORM_LENGTH) {
        index_r = 0;
    }

    float frac = this->Accmulation - (float)index_l;

    float y0 = (float)this->WaveForm[index_l];
    float y1 = (float)this->WaveForm[index_r];

    uint32_t result = (uint32_t)(y0 + frac * (y1 - y0));
    return (uint16_t)(result > 4095 ? 4095 : result); // 强制限幅
}

// FM 和 AM 的实现方法趋同,但是调用方法不同,所引用的数据存储方式位置和结构都不同,有待修改.
void OSC::OSC_FM(uint16_t Max){
    // uint16_t TemporaryValue = this->FM_Coeff;
    // this->ActualFreq = this->TargetFreq + (this->TargetFreq * (float)TemporaryValue / (float)Max);
    this->ActualFreq = this->TargetFreq + (this->TargetFreq * this->FM_Coeff);
}

// float OSC::OSC_AM(uint16_t Max){
//     uint16_t TemporaryValue = *(this->AM_Coeff);
//     return (float)TemporaryValue / (float)Max;
// }

void OSC::OSC_WaveFormSelect(uint8_t WaveFormIndex){
    this->WaveForm = (uint16_t *)WaveFormList[WaveFormIndex];
}


// OSC OSC1(&MediatorTest);
// OSC OSC1;

// /* ---- DAC输出 ---- */
// uint16_t *OutBuffer = OSC1.Buffer; // 此处将OSC1的缓冲区直接链接至输出缓冲区


// RTOS任务待补充