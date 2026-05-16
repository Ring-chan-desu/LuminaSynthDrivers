#include "LFO.h"

#include "../System/Common/Common.h"

// #include <cstdint> // Moved to Common.h
// #include <stdint.h> // Moved to Common.h
#include <math.h>

// #include "../System/Interface/Lumina_Interface.h" // Redundant include; LFO.h already provides required declarations
#include "../WaveForm/WaveForm.h"
#include "../System/Mediator/Mediator.h"
#include "../System/Knob/Knob.h"
#include "stm32f4xx_hal_adc.h"
#include "../Test/TestModules/USART_Debug/UDB.h"

extern osThreadId_t LFO_Update_TaskHandle;
extern osSemaphoreId_t LFO_Update_SemHandle;

extern LFO LFO1;

/* ---- LFO ---- */

void LFO::LFO_StepCalculate(){
    this->Step = this->ActualFreq * FM_CONSTANT * 50;
    // this->LFO_FM(CAPTURE_UPPER_LIMIT);
}

void LFO::LFO_Update(){
    // this->prescalerCount++;
    // if (this->prescalerCount >= 0) {
        this->prescalerCount = 0;
        this->LFO_Accmulate();
        this->LFO_StepCalculate();
        uint16_t interpolated = this->LFO_Lerp();   //  插值之后得到结果
        // float currentWeight = (interpolated / 4095.0f) * 2.0f - 1.0f;   //  直接计算成权重
        if (m != nullptr) {
            // m->Mediator_Publish(Topics::LFO_DATA, currentWeight);
        }
        // 调试输出
        UDB_Send_FireWater("lfo", interpolated);
    // }
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

void LFO::LFO_FM(uint16_t Max){
    // LFO 简化，无 FM
    this->ActualFreq = this->TargetFreq;
}

// LFO LFO1(&MediatorTest);

/* ---- LFO Update Semaphore ---- */
osSemaphoreId_t LFO_Update_SemHandle;

void LFO_Update_SemInit(void)
{
    const osSemaphoreAttr_t LFO_Update_SemAttr = {
        .name = "LFO_Update_Sem"
    };
    // 创建二进制信号量：初始值0，最大值1
    LFO_Update_SemHandle = osSemaphoreNew(1, 0, &LFO_Update_SemAttr);
}

/* ---- LFO Update Task ---- */
osThreadId_t LFO_Update_TaskHandle;

void LFO_Update_Task(void *argument)
{
    for(;;)
    {
        osSemaphoreAcquire(LFO_Update_SemHandle, osWaitForever);
        osDelay(50);
        LFO1.LFO_Update();
    }
}

void LFO_Update_TaskInit(void)
{
    const osThreadAttr_t LFO_Update_TaskAttr = {
        .name = "LFO_Update_Task",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };
    LFO_Update_TaskHandle = osThreadNew(LFO_Update_Task, NULL, &LFO_Update_TaskAttr);
}