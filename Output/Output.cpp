#include "../System/Common/Common.h"    //  通用头文件包含与宏定义

#include "../OSC/OSC.h" //  OSC类方法
#include "../System/Mediator/Mediators.h"    // Mediator类
#include "../ADSR/ADSR.h"   // ADSR类方法
#include "../LFO/LFO.h" // LFO类方法

#include "./Output.h"   //  自身头文件

#include "../System/Interface/Lumina_Interface.h"   //  提供实例化的对象
extern "C"{
    #include "arm_math.h"
}

uint16_t    OutBuffer_q15[FULL_BUFFER_LENGTH] = {0};
float       OutBuffer_f32[FULL_BUFFER_LENGTH] = {0};

void Output_generalOutBufferFill(uint8_t halfFlag) {
    // ParamMediator::GetInstance().ParamSyncBuffersInISR(); // updated to current ParamMediator singleton naming
    OSC1.OSC_BufferFill(halfFlag);
    // arm_copy_f32(OSC1.Buffer, OutBuffer_f32, FULL_BUFFER_LENGTH);

    LFO1.LFO_BufferFill(halfFlag);

    ADSR1.ADSR_BufferFill(halfFlag);
    // 如下参数动态变化,实现自由路由,通过ParamMediator订阅实现
    // arm_mult_f32(OSC1.Buffer, LFO1.Buffer, OutBuffer_f32, FULL_BUFFER_LENGTH);  //  向量化乘法
    // arm_mult_f32(OSC1.Buffer, ADSR1.Buffer, OutBuffer_f32, FULL_BUFFER_LENGTH);  //  向量化乘法
    arm_copy_f32(OSC1.Buffer, OutBuffer_f32, FULL_BUFFER_LENGTH);

    for (uint32_t i = 0; i < FULL_BUFFER_LENGTH; i++) {   //  最后的浮点数到整数的转换
        // ✨ 如果你需要四舍五入，直接在浮点数后面 +0.5f，然后强转 uint16_t！
        // C++ 强转成整型时会自动把小数部分切掉，加了 0.5f 就等于完美的四舍五入！
        OutBuffer_q15[i] = (uint16_t)(OutBuffer_f32[i] + 0.5f);
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