#include "./Lumina_Interface.h"
#include "../OSC/OSC.h"
#include "../System/Mediator/Mediator.h"
#include "../ADSR/ADSR.h"

#include <stdint.h>
// #include "../Knob/Knob.h"

extern uint32_t *OutBuffer;
extern ADSR ADSR1;
/* ---- 实例化 ---- */
OSC OSC1(&(Mediator::GetInstance()));

void Lumina_Interface_Init(void) {
    Mediator::GetInstance();
    ADSR1.ADSR_Init();
    // static ADSR ADSR1(&(Mediator::GetInstance()));
    // static OSC OSC1(&(Mediator::GetInstance()));
    // OSCGeneralInit();
}

extern "C" uint16_t* Lumina_Interface_GetOutBuffer(void) {
    return &(OSC1.Buffer[0]);   //  临时值
}

float Lumina_Interface_ADSR_GetValue(float CurrentX) {
    return ADSR1.ADSR_FunctionValueCalculate(CurrentX);
}

/* ---- DAC回调部分 ---- */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [0~255]
{
    OSC1.OSC_BufferFill(0);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [256~511]
{
    OSC1.OSC_BufferFill(1);
}

/* ---- I2S 回调部分 ---- */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) // 处理 Buffer 前半部分
{
    OSC1.OSC_BufferFill(0);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) // 处理 Buffer 后半部分
{
    OSC1.OSC_BufferFill(1);
}