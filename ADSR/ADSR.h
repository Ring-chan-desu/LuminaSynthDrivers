#ifndef __ADSR_H__
#define __ADSR_H__

#include <stdint.h>
#include <stdbool.h>

#include "ADSR.h"
#include "adsrMatrix.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
/* ----TracerPoint对象---- */
// typedef struct
// {
//     uint8_t TargetX;
//     uint8_t LimitX;
//     bool TracerRunFlag;
// }TracerPoint;

/* ----状态机---- */
typedef enum
{
    ADSR_STATE_REST,
    ADSR_STATE_NORMAL,
    ADSR_STATE_RELEASE,
}ADSR_STATE;

/* ----点结构体---- */
typedef struct
{
    int16_t x;
    int16_t y;
}adsrPoint;

extern uint16_t ADC_Buffer[4];
extern uint16_t ADC_Buffer_Processed[4];

extern bool TracerRunFlag;

extern adsrPoint p2;
extern adsrPoint p3;
extern adsrPoint p4;

extern uint8_t ADSRState; // ADSR状态

extern uint8_t TargetX;
extern uint16_t LimitX;

int16_t ADSR_LineGenerate(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t x);
uint16_t ADSR_FunctionValueCalculate(adsrPoint p2, adsrPoint p3, adsrPoint p4, uint16_t x);
void ADSR_PointPrint(uint16_t x, entry Entry, uint16_t Offset);

/* ----RTOS---- */

/*Tasks*/
void ADSR_ADCPulse_TaskInit(void);
void ADSR_DrawEnvelopeTracer_TaskInit(void);
/*Sems*/
void ADSR_MIDI_ON_STATE_SemInit(void);
void ADSR_MIDI_OFF_STATE_SemInit(void);

extern osSemaphoreId_t ADSR_MIDI_ON_STATE_SemHandle;
extern osSemaphoreId_t ADSR_MIDI_OFF_STATE_SemHandle;

#endif
