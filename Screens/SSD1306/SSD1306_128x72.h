#ifndef __SSD1306_128X72_H__
#define __SSD1306_128X72_H__

#include "main.h"
#include <stdint.h>

#define ADDRESS             0x78
#define FULL_ARRAY_LENGTH   1152

/*用于清零的常数组*/
extern uint8_t ClearArray[FULL_ARRAY_LENGTH + 1];

/*可自定义测试数组*/
extern uint8_t TestArray[];
extern uint8_t TestArrayLength;
extern uint8_t TestArray2[128];//三角波测试数组

/*显存*/
extern uint8_t GRAM[128][9];

typedef struct
{
    uint8_t Page;//页地址
    uint8_t Offset;//偏移量
}Pixel;

void Screen_Init();

void Screen_SendCommand(uint8_t Command);
void Screen_SendData(uint8_t Data);
void Screen_SetSite(uint8_t x, uint8_t page);

void Screen_Clear();
void Screen_InitTestArray(uint8_t* TestArray);
Pixel Screen_NumberProcess(uint8_t Number);
void Screen_GramSetValue(uint8_t* Array, uint8_t ArrayLength, uint8_t(*GRamArray)[9]);

#endif
