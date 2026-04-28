#ifndef __ADSR_H__
#define __ADSR_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "main.h"
#include "../System/Interface/Lumina_Interface.h"

/* ---- 2. C/C++ 通用类型声明 ---- */
typedef struct {
    float x;
    float y;
}ADSR_Point;    //  点结构体

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
class ADSR{
    private:
    public:
        ADSR()
        :   /*points{ADSR_Point{10.0,ADSR_HEIGHT}, ADSR_Point{20.0,0.0}, ADSR_Point{20.0,0.0}}*/
            p2({10.0f, ADSR_HEIGHT}),
            p3({20.0f, ADSR_HEIGHT - 30.0f}),
            p4({30.0f, 30.0f})  //  暂时的点初始化
        {

        }
        // ADSR_Point points[ADSR_POINT_COUNT];    //  起始点和终止点略去
        ADSR_Point p2;
        ADSR_Point p3;
        ADSR_Point p4;

        float ADSR_Accmulation = 0.0f;
        float ADSR_Step = 0.0f;

        float ADSR_LineGenerate(float x1, float y1, float x2, float y2, float x);
        float ADSR_FunctionValueCalculate(ADSR_Point p2, ADSR_Point p3, ADSR_Point p4, float x);

};

extern ADSR ADSR1;
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __ADSR_H__ */