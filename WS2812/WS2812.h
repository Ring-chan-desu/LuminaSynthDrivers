#ifndef __WS2812_H__
#define __WS2812_H__

#include "main.h"
#include "tim.h"
#include <stdint.h>

#define CODE_0         25
#define CODE_1         65
#define RESET_CODE     0

#define COUNT           4

extern uint8_t WS2812_TempGRAM[COUNT][3];
extern char WS2812_TempStrings[COUNT][7];
extern uint8_t WS2812_TempGRAM2[COUNT][3];

void WS2812_RamUpdate(uint8_t (*GRAM)[3]);
void WS2812_HexColorToOctValue(uint8_t(*GRam)[3], char(*String)[7]);

#endif
