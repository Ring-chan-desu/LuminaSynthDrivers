#ifndef __ADSR_MATRIX_H__
#define __ADSR_MATRIX_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "../ADSR.h"

#include <stdint.h>
#include <string.h>

#define MATRIX_WIDTH    64  // 矩阵宽
#define MATRIX_HEIGHT   36  // 矩阵高

/* --- 2. C/C++ 通用类型声明 --- */
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

/* ---- 矩阵点结构体---- */
typedef struct
{
    int16_t x;
    int16_t y;
}Matrix_Point;

#ifdef __cplusplus
/* --- 3. 仅 C++ 可见的类定义 --- */
class ADSR;

class ADSR_Matrix {
    private:
        ADSR* master;

    public:
        ADSR_Matrix(ADSR* master) :        
            master(master),
            Matrix_LineIndex(0)
        {
            memset(MatrixBuffer, 0, sizeof(MatrixBuffer));
        }

        Matrix_Point StartEntry; // 起始点
        entry Entry; // 数对结构体
        uint16_t Matrix_LineIndex; // 矩阵横坐标
        uint16_t MatrixBuffer[MATRIX_HEIGHT]; // 矩阵缓冲区 uint16_t是单个颜色块,调整DMA位宽就不用拆分发送了

        void ADSR_Matrix_Init(uint8_t x, uint8_t y);
        void ADSR_Matrix_CurvePaint();
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
void test(); // 测试函数

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __ADSR_MATRIX_H__ */