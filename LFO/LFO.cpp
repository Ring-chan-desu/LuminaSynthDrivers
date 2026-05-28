#include "LFO.h"

#include "../System/Common/Common.h"

// #include <cstdint> // Moved to Common.h
// #include <stdint.h> // Moved to Common.h

// #include "../System/Interface/Lumina_Interface.h" // Redundant include; LFO.h already provides required declarations
#include "../WaveForm/WaveForm.h"
#include "../System/Mediator/Mediators.h"
#include "../System/Knob/Knob.h"
#include "stm32f4xx_hal_adc.h"
#include "../Test/TestModules/USART_Debug/UDB.h"

extern LFO LFO1;

/* ---- LFO ---- */

void LFO::LFO_StepCalculate(){
    this->Step = this->ActualFreq * FM_CONSTANT;
    // this->LFO_FM(CAPTURE_UPPER_LIMIT);
}

void LFO::LFO_BufferFill(uint8_t HalfFlag){
    // 明确指定起始位置
    float* Start = (HalfFlag == 0) ? &this->Buffer[0] : &this->Buffer[HALF_BUFFER_LENGTH];
    
    for(int i = 0 ; i < HALF_BUFFER_LENGTH ; i ++){
        Start[i] = (this->LFO_Lerp() * this->AM_Coeff) / 4096.0f;   //  输出权重
        this->LFO_Accmulate();
    }
    
    if (HalfFlag) {
        this->LFO_StepCalculate();
    }
}

void LFO::LFO_Accmulate(void){
    this->Accmulation += this->Step;
    if (this->Accmulation >= (float)WAVEFORM_LENGTH) {
        this->Accmulation -= (float)WAVEFORM_LENGTH;
    }
}

uint16_t LFO::LFO_Lerp(){
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