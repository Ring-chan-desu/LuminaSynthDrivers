#include "OSC.H"
#include "OSC.h"

#include "main.h"
#include "stm32f407xx.h"

#include <stdint.h>
#include <math.h>

#include "../WaveForm/WaveForm.h"
#include "./OSC.h"
#include "stm32f4xx_hal_adc.h"

extern uint16_t ADC_Buffer[4];

/* ---- 振荡器 ---- */
void OSC::OSC_Init(void){
    this->WaveForm  = SineWave;
    this->PhaseFlag = 0;

    this->TargetFreq = 144.0f;
    this->ActualFreq = this->TargetFreq;
    this->Step = 9.386667f;
}

void OSC::OSC_StepCalculate(){
    this->Step = this->ActualFreq * FM_CONSTANT;
    this->OSC_FM(&ADC_Buffer[0], 4096);
}

void OSC::OSC_BufferFill(uint8_t HalfFlag){
    // 明确指定起始位置
    uint16_t* Start = (HalfFlag == 0) ? &this->Buffer[0] : &this->Buffer[HALF_BUFFER_LENGTH];
    
    for(int i = 0 ; i < HALF_BUFFER_LENGTH ; i ++){
        Start[i] = this->OSC_Lerp() * this->OSC_AM(&ADC_Buffer[1], 4096);
        this->OSC_Accmulate();
    }
    
    if (HalfFlag) {
        this->OSC_StepCalculate();
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
void OSC::OSC_FM(uint16_t* pCoeff, uint16_t Max){
    uint16_t TemporaryValue = *pCoeff;
    this->ActualFreq = this->TargetFreq + (this->TargetFreq * (float)TemporaryValue / (float)Max);
}

float OSC::OSC_AM(uint16_t* pCoeff, uint16_t Max){
    uint16_t TemporaryValue = *pCoeff;
    return (float)TemporaryValue / (float)Max;
}

OSC OSC1;

void OSCGeneralInit(){
    OSC1.OSC_Init();
}

/* ---- DAC输出 ---- */
uint16_t *OutBuffer = OSC1.Buffer; // 此处将OSC1的缓冲区直接链接至输出缓冲区

/* ---- 回调部分 ---- */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [0~255]
{
    OSC1.OSC_BufferFill(0);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [256~511]
{
    OSC1.OSC_BufferFill(1);
}

/* ---- ADC捕获 ---- */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {

    }
}