#include "../System/Common/Common.h"    //  通用头文件包含与宏定义

#include "../OSC/OSC.h" //  OSC类方法
#include "../System/Mediator/Mediators.h"    // Mediator类
#include "../ADSR/ADSR.h"   // ADSR类方法
#include "../LFO/LFO.h" // LFO类方法

#include "./Output.h"   //  自身头文件

#include "../System/Interface/Lumina_Interface.h"   //  提供实例化的对象

uint16_t    OutBuffer_q15[FULL_BUFFER_LENGTH] = {0};
float       OutBuffer_f32[FULL_BUFFER_LENGTH] = {0};

void Output_generalOutBufferFill(uint8_t halfFlag) {
    // 起点指定
    float* startF32 = (halfFlag == 0) ? &OutBuffer_f32[0] : &OutBuffer_f32[HALF_BUFFER_LENGTH];
    uint16_t* startQ15 = (halfFlag == 0) ? &OutBuffer_q15[0] : &OutBuffer_q15[HALF_BUFFER_LENGTH];

    // 全0填充
    arm_fill_f32(0, startF32, HALF_BUFFER_LENGTH);

    // 更新步长
    OSC1.OSC_update();

    // 填写缓冲区
    OSC1.OSC_calculate();

    // 复制缓冲区
    arm_copy_f32(oscGeneralOutBuffer, startF32, HALF_BUFFER_LENGTH);

    // 四舍五入
    for (uint16_t i = 0; i < HALF_BUFFER_LENGTH; i++) {
        startQ15[i] = (uint16_t)(startF32[i] + 0.5f);
    }
}

/* ---- DAC回调部分 ---- */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [0~255]
{
    // OSC1.OSC_BufferFill(0);
    Output_generalOutBufferFill(0);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [256~511]
{
    // OSC1.OSC_BufferFill(1);
    Output_generalOutBufferFill(1);
}

/* ---- I2S 回调部分 ---- */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) // 处理 Buffer 前半部分
{
    // OSC1.OSC_BufferFill(0);
    Output_generalOutBufferFill(0);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) // 处理 Buffer 后半部分
{
    // OSC1.OSC_BufferFill(1);
    Output_generalOutBufferFill(1);
}