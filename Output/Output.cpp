#include "../System/Common/Common.h"    //  通用头文件包含与宏定义

#include "./Output.h"

#include "../System/Interface/Lumina_Interface.h"

/* ---- DAC回调部分 ---- */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [0~255]
{
    // OSC1.OSC_BufferFill(0);
    Lumina_Interface_Out_BufferFill(0);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) // 处理 Buffer [256~511]
{
    // OSC1.OSC_BufferFill(1);
    Lumina_Interface_Out_BufferFill(1);
}

/* ---- I2S 回调部分 ---- */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) // 处理 Buffer 前半部分
{
    // OSC1.OSC_BufferFill(0);
    Lumina_Interface_Out_BufferFill(0);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) // 处理 Buffer 后半部分
{
    // OSC1.OSC_BufferFill(1);
    Lumina_Interface_Out_BufferFill(1);
}