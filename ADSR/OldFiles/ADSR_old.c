#include <stdint.h>
#include <stdbool.h>

#include "ADSR.h"
#include "adsrMatrix.h"
#include "RTOSBridge.h"

#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
// ADSR四个旋钮取值
// 三个点
// 状态
//

/* Attributes */
/*-----------------------------------*/
/* ----绘制标志位----*/
bool TracerRunFlag = true;

// adsrPoint p1 = {0,0};
adsrPoint p2 = {0,MATRIX_HEIGHT};
adsrPoint p3 = {64,MATRIX_HEIGHT};
adsrPoint p4 = {64,0};
// adsrPoint p5 = {256,0}
// as default value

/* ----ADSR---- */
uint16_t a;
uint16_t d;
uint16_t s;
uint16_t r;

uint8_t ADSRState = ADSR_STATE_REST; // ADSR状态

uint8_t TargetX = 0; // 目标X坐标
uint16_t LimitX  = 0; // 上限X坐标

/* ----ADC数组---- */
uint16_t ADC_Buffer[4] = {0};
uint16_t ADC_Buffer_Processed[4] = {0};
/*-----------------------------------*/

/**
 * @brief 单线段函数解析式与求值
 */
int16_t ADSR_LineGenerate(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t x)
{
    if (x <= x1) return y1;
    if (x >= x2) return y2;

    int16_t Deltax = x2 - x1;
    if (Deltax == 0) return y1;

    int16_t Deltay = y2 - y1;

    int16_t y = y1 + ( Deltay * (x - x1) ) / Deltax;

    return y;
}

/**
 * @brief 根据给定点集和x坐标实时生成对应函数值
 * 
 * @param p2
 * @param p3
 * @param p4
 * @param x 当前的横坐标
 */
uint16_t ADSR_FunctionValueCalculate(adsrPoint p2, adsrPoint p3, adsrPoint p4, uint16_t x)
{  
    if(0 <= x && x < p2.x)
    {
        return ADSR_LineGenerate(0, 0, p2.x, p2.y, x);
    }
    else if(p2.x <= x && x < p3.x)
    {
        return ADSR_LineGenerate(p2.x, p2.y, p3.x, p3.y, x);
    }
    else if(p3.x <= x && x < p4.x)
    {
        return ADSR_LineGenerate(p3.x, p3.y, p4.x, p4.y, x);
    }
    else if(p4.x <= x && x < MATRIX_WIDTH)
    {
        return ADSR_LineGenerate(p4.x, p4.y, MATRIX_WIDTH, 0, x);
    }
    else
    {
        return 0;
    }
}

/* ----RTOS---- */

/* ---- ADSR ADCPulse Task ---- */
osThreadId_t ADSR_ADCPulse_TaskHandle;

void ADSR_ADCPulse_Task(void *argument)
{
    for(;;)
    {
        osSemaphoreAcquire(adsrMatrix_DisplayPeriod_SemHandle, osWaitForever);
        /* ----获取ADC捕获值并处理---- */
        for(uint8_t SerialNumber = 0 ; SerialNumber < 4 ; SerialNumber ++)
        {
            ADC_Buffer_Processed[SerialNumber] = ADC_Buffer[SerialNumber] >> 8;
        }

        a = (ADC_Buffer_Processed[0] * MATRIX_WIDTH)         / 15;
        d = (ADC_Buffer_Processed[1] * (MATRIX_WIDTH - a) )  / 15 + a;
        s = (ADC_Buffer_Processed[2] * MATRIX_HEIGHT)         / 15;
        r = (ADC_Buffer_Processed[3] * (MATRIX_WIDTH - d) )  / 15 + d;

        /* ----对点赋值---- */
        // adsrPoint p1 = {0,0};
        p2 = (adsrPoint){a,MATRIX_HEIGHT};
        p3 = (adsrPoint){d,s};
        p4 = (adsrPoint){r,0};
        // adsrPoint p5 = {MATRIX_WIDTH,0};
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buffer, 4);
    }
}

void ADSR_ADCPulse_TaskInit(void)
{
    const osThreadAttr_t ADSR_ADCPulse_TaskAttr = {
        .name = "ADSR_ADCPulse_Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };
    ADSR_ADCPulse_TaskHandle = osThreadNew(ADSR_ADCPulse_Task, NULL, &ADSR_ADCPulse_TaskAttr);
}

/* ---- ADSR DrawEnvelopeTracer Task ---- */
osThreadId_t ADSR_DrawEnvelopeTracer_TaskHandle;

void ADSR_DrawEnvelopeTracer_Task(void *argument)
{
    for(;;)
    {
        // --- 1. 外部信号触发状态切换 ---
        if (osSemaphoreAcquire(ADSR_MIDI_ON_STATE_SemHandle, 0) == osOK)
        {
            ADSRState = ADSR_STATE_NORMAL;
            // 触发瞬间初始化
            TargetX = 0;
            LimitX  = d;
            TracerRunFlag = true; 
        }
        else if (osSemaphoreAcquire(ADSR_MIDI_OFF_STATE_SemHandle, 0) == osOK)
        {
            ADSRState = ADSR_STATE_RELEASE;
            // 释放瞬间初始化
            TargetX = d; 
            LimitX = r;
            TracerRunFlag = true;
        }

        // --- 2. 状态机内部自动流转 ---
        // 这里主要处理当 TargetX 到达上限后的动作
        if (TracerRunFlag)
        {
            TargetX++; 
            
            if (TargetX >= LimitX)
            {
                TargetX = LimitX;
                
                // 如果到达了 Release 的终点，进入 REST
                if (ADSRState == ADSR_STATE_RELEASE) {
                    ADSRState = ADSR_STATE_REST;
                    TracerRunFlag = false;
                }
                // 如果是 NORMAL 阶段结束，可能需要切换到 DECAY 或保持
                else if (ADSRState == ADSR_STATE_NORMAL) {
                    // ADSRState = ADSR_STATE_DECAY; // 举例
                }
            }
        }

        osDelay(20);
    }
}

void ADSR_DrawEnvelopeTracer_TaskInit(void)
{
    const osThreadAttr_t ADSR_DrawEnvelopeTracer_TaskAttr = {
        .name = "ADSR_DrawEnvelopeTracer_Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };
    ADSR_DrawEnvelopeTracer_TaskHandle = osThreadNew(ADSR_DrawEnvelopeTracer_Task, NULL, &ADSR_DrawEnvelopeTracer_TaskAttr);
}

/* ---- ADSR MIDI_ON_STATE Semaphore ---- */
osSemaphoreId_t ADSR_MIDI_ON_STATE_SemHandle;

void ADSR_MIDI_ON_STATE_SemInit(void)
{
    const osSemaphoreAttr_t ADSR_MIDI_ON_STATE_SemAttr = {
        .name = "ADSR_MIDI_ON_STATE_Sem"
    };
    // 创建二进制信号量：初始值0，最大值1
    ADSR_MIDI_ON_STATE_SemHandle = osSemaphoreNew(1, 0, &ADSR_MIDI_ON_STATE_SemAttr);
}

/* ---- ADSR MIDI_OFF_STATE Semaphore ---- */
osSemaphoreId_t ADSR_MIDI_OFF_STATE_SemHandle;

void ADSR_MIDI_OFF_STATE_SemInit(void)
{
    const osSemaphoreAttr_t ADSR_MIDI_OFF_STATE_SemAttr = {
        .name = "ADSR_MIDI_OFF_STATE_Sem"
    };
    // 创建二进制信号量：初始值0，最大值1
    ADSR_MIDI_OFF_STATE_SemHandle = osSemaphoreNew(1, 0, &ADSR_MIDI_OFF_STATE_SemAttr);
}
/* Pic was made by "CodeImg" https://github.com/subframe7536/vscode-codeimg.git */
/* おやすみのキスして? */