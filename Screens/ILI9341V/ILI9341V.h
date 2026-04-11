#ifndef __ILI9341V_H__
#define __ILI9341V_H__

#include "main.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"

#include <stdint.h>

/* 颜色结构体 */
typedef struct 
{
    uint8_t HighBits;
    uint8_t LowBits;
}color;

/* 点结构体 */
typedef struct
{
    uint16_t x;
    uint16_t y;
}point;

/* 数字结构体 */
typedef struct
{

    uint8_t HighBits;   // 高四位
    uint8_t LowBits;    // 低四位

}number;

void ILI9341V_WriteCommand(uint8_t Command);
void ILI9341V_WriteData(uint8_t Data);
void ILI9341V_Init();
void ILI9341V_Reset();
void ILI9341V_Clear();

number ILI9341V_SingleNumberDevider(uint16_t Number);
void ILI9341V_ColumnAddressSet(uint16_t StartColumnNumber, uint16_t EndColumnNumber);
void ILI9341V_RowAddressSet(uint16_t StartRowNumber, uint16_t EndRowNumber);
void ILI9341V_SetRange(point StartPoint, point Endpoint);
uint8_t ILI9341V_HexCharToOctValue(char Character);
color ILI9341V_HexColorToOctValue(char* String);

#endif
