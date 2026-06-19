#include "OSC.h"

#include "../System/Common/Common.h"

// #include "./OSC.h" // Redundant duplicate include
// #include "../System/Interface/Lumina_Interface.h" // Redundant include; OSC.h and Common.h provide required declarations
#include "../System/Mediator/Mediators.h"   //  中转站
#include "../WaveForm/WaveForm.h"   //  波表
#include "../System/Knob/Knob.h"
#include "arm_math.h"
#include <math.h>
#include <stdint.h>
#include "../MIDI/midi-in/midiIn.h" //  MIDI

float oscGeneralOutBuffer[HALF_BUFFER_LENGTH] = {0};  //  所有OSC唯一共用的缓冲区

void OSC::OSC_StepCalculate(void){  
    float fmFactor = this->in_oscFM;
    for (uint8_t i = 0; i < MAX_POLY_NUM; i++) {
        if (voiceAllocator1.paramGroup[i].gate == true) {
            float actualFreq = voiceAllocator1.paramGroup[i].targetFreq * (1.0f + fmFactor); 
            this->oscSteps[i] = actualFreq * FM_CONSTANT;
        } else {
            this->oscSteps[i] = 0.0f;
        }
    }
}

void OSC::OSC_Accmulate(void){  // 统一累加
    // for (commonParam& instance : voiceAllocator1.paramGroup) {
    for (uint8_t i = 0; i < MAX_POLY_NUM; i++) {
        this->oscAccmulators[i] += voiceAllocator1.paramGroup[i].step;
        if (this->oscAccmulators[i] >= (float)WAVEFORM_LENGTH) {
            this->oscAccmulators[i] -= (float)WAVEFORM_LENGTH;
        }
    }
}

void OSC::OSC_calculate(void){  
    // arm_fill_f32(0.0f, oscGeneralOutBuffer, HALF_BUFFER_LENGTH);    //  缓冲区清零逻辑,暂时注释掉
    uint8_t activeCount = 0;
    float amFactor = this->in_oscAM;
    
    for (uint8_t i = 0; i < MAX_POLY_NUM; i++){
        if (voiceAllocator1.paramGroup[i].gate == false) {
            continue;
        } else {
            activeCount++;
        }

        float localStep = this->oscSteps[i];    // 步长锁存
        float localAcc  = this->oscAccmulators[i]; // 累加器锁存

        //  线性插值集成,避免了函数调用开销
        for (int j = 0; j < HALF_BUFFER_LENGTH; j++) {
            
            uint16_t index_l = (uint16_t)localAcc;
            uint16_t index_r = index_l + 1;
            if (index_r >= WAVEFORM_LENGTH) { index_r = 0; }

            float frac = localAcc - (float)index_l;
            float y0 = (float)this->WaveForm[index_l];
            float y1 = (float)this->WaveForm[index_r];

            uint32_t result = (uint32_t)(y0 + frac * (y1 - y0));
            
            localAcc += localStep;
            if (localAcc >= (float)WAVEFORM_LENGTH) {
                localAcc -= (float)WAVEFORM_LENGTH;
            }

            uint16_t outSample = (uint16_t)(result > 4095 ? 4095 : result);
            
            oscGeneralOutBuffer[j] += (outSample * amFactor) * 0.5f;
        }
        
        this->oscAccmulators[i] = localAcc;
    }
    
    if (activeCount == 0) { 
        return; //  活跃数==0就不用再做了
    }
    
    float weight = 1.0f / (float)activeCount;
    arm_scale_f32(oscGeneralOutBuffer, weight, oscGeneralOutBuffer, HALF_BUFFER_LENGTH);
}


void OSC::OSC_update(void){
    this->OSC_StepCalculate();
}