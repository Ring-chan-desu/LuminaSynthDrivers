#ifndef __ADSR_H__
#define __ADSR_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "main.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "../ADSR_Matrix/ADSR_Matrix.h"

#include <stdint.h>
#include <stdbool.h>

/* --- 2. C/C++ 通用类型声明 --- */
typedef struct
{
    int16_t x;
    int16_t y;
}adsrPoint;

typedef enum
{
    ADSR_STATE_REST,
    ADSR_STATE_NORMAL,
    ADSR_STATE_RELEASE,
}ADSR_STATE;

#ifdef __cplusplus
/* --- 3. 仅 C++ 可见的类定义 --- */
class ADSR {
    public:
        adsrPoint p2;
        adsrPoint p3;
        adsrPoint p4;

        ADSR_STATE ADSRState; // ADSR状态

        int16_t ADSR_LineGenerate(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t x);
        uint16_t ADSR_FunctionValueCalculate(adsrPoint p2, adsrPoint p3, adsrPoint p4, uint16_t x);
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 4. 供 C 调用的公开 API (Wrapper) --- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __ADSR_H__ */