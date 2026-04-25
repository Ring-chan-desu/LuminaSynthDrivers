#ifndef __MATRIX_H__
#define __MATRIX_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "main.h"

#include <stdint.h>
#include <stdbool.h>

    /* ---- Macros ---- */
    #define MATRIX_WIDTH  16    //  矩阵宽度
    #define MATRIX_HEIGHT 16    //  矩阵高度

/* ---- 2. C/C++ 通用类型声明 ---- */
typedef struct {
    uint16_t x;
    uint16_t y;
}Matrix_Point; // 矩阵点结构体

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
class Matrix
{
    public:
        Matrix_Point StartPoint;
        Matrix_Point EndPoint;  //  会比实际填入的长宽多三个像素,因为要有个2px的边框

        void Matrix_Init(Matrix_Point StartPoint, Matrix_Point EndPoint, bool IsFrame);
}

/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __MATRIX_H__ */