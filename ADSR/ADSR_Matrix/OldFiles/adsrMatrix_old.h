#ifndef __ADSRMARTIX_H__
#define __ADSRMARTIX_H__

#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#define MATRIX_WIDTH    64
#define MATRIX_HEIGHT   36
/* ----矩阵对象化---- */
// typedef struct
// {
//     uint16_t LineIndex;
//     uint8_t Buffer[MATRIX_HEIGHT * 2];
//     uint8_t x;
//     uint8_t y;
// }ADSR_Matrix;

/* ----函数值结构体---- */
typedef struct
{
    uint16_t Number;
    uint16_t Index;
}coordinate;

/* ----数对结构体---- */
typedef struct
{
    coordinate CurrentNumber;
    coordinate LastNumber;
}entry;

/* ----矩阵缓冲区---- */
extern uint8_t MatrixBuffer[MATRIX_HEIGHT * 2];

/* ----横坐标---- */
extern uint16_t Matrix_LineIndex;

void ADSR_Matrix_Init(uint8_t x, uint8_t y);
void ADSR_Matrix_CurvePaint();

/* ----RTOS---- */

/*Sems*/
/*Functions*/
void adsrMatrix_DisplayPeriod_SemInit(void);
void adsrMatrix_DMAComplete_SemInit(void);
/*Announcements*/
extern osSemaphoreId_t adsrMatrix_DisplayPeriod_SemHandle;
extern osSemaphoreId_t adsrMatrix_DMAComplete_SemHandle;

/*Tasks*/
void adsrMatrix_DMAEnable_TaskInit(void);

#endif
