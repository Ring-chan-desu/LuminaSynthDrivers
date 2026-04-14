#include "ADSR.h"
#include "main.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include <stdint.h>
#include <stdbool.h>

/* ---- ADSR 函数实现 ---- */
int16_t ADSR::ADSR_LineGenerate(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t x)
{
    if (x <= x1) return y1;
    if (x >= x2) return y2;

    int16_t Deltax = x2 - x1;
    if (Deltax == 0) return y1;

    int16_t Deltay = y2 - y1;

    int16_t y = y1 + ( Deltay * (x - x1) ) / Deltax;
    return y;
}

uint16_t ADSR::ADSR_FunctionValueCalculate(adsrPoint p2, adsrPoint p3, adsrPoint p4, uint16_t x)
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

ADSR ADSR1;