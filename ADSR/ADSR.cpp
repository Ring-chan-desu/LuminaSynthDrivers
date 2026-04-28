#include "./ADSR.h"
#include "../System/Interface/Lumina_Interface.h"

/**
 * @brief 单线段函数解析式与求值 (浮点数版本)
 * @param x1 起始点横坐标
 * @param y1 起始点纵坐标
 * @param x2 终止点横坐标
 * @param y2 终止点纵坐标
 * @param x  当前查询的横坐标
 * @return 对应的纵坐标函数值 (float)
 */
float ADSR::ADSR_LineGenerate(float x1, float y1, float x2, float y2, float x) {
    // 1. 边界检查：若 x 在区间外，直接返回临界值
    if (x <= x1) return y1;
    if (x >= x2) return y2;

    // 2. 计算横轴差值，防止除以 0 导致的崩溃
    float Deltax = x2 - x1;
    if (Deltax == 0.0f) return y1;

    // 3. 计算纵轴差值
    float Deltay = y2 - y1;

    // 4. 线性插值计算：y = y1 + k * (x - x1)
    float y = y1 + (Deltay * (x - x1)) / Deltax;

    return y;
}

/**
 * @brief 根据给定点集和x坐标实时生成对应函数值 (浮点数版本)
 * @param p2 第一拐点 (Attack 结束, Decay 开始)
 * @param p3 第二拐点 (Decay 结束, Release 开始)
 * @param p4 第三拐点 (Release 结束)
 * @param x  当前的横坐标 (时间轴)
 * @return 对应的纵坐标函数值 (幅度)
 */
float ADSR::ADSR_FunctionValueCalculate(ADSR_Point p2, ADSR_Point p3, ADSR_Point p4, float x)
{  
    // 1. Attack 阶段：从原点 (0,0) 到 p2
    if(x >= 0.0f && x < p2.x)
    {
        return this->ADSR_LineGenerate(0.0f, 0.0f, p2.x, p2.y, x);
    }
    // 2. Decay 阶段：从 p2 到 p3
    else if(x >= p2.x && x < p3.x)
    {
        return this->ADSR_LineGenerate(p2.x, p2.y, p3.x, p3.y, x);
    }
    // 3. Sustain 阶段：从 p3 到 p4
    // 注：在你的逻辑中，p3 到 p4 实际上是维持或释放前的过渡
    else if(x >= p3.x && x < p4.x)
    {
        return this->ADSR_LineGenerate(p3.x, p3.y, p4.x, p4.y, x);
    }
    // 4. Release 阶段：从 p4 到 屏幕终点 (向 0 归集)
    else if(x >= p4.x && x < (float)ADSR_WIDTH)
    {
        return this->ADSR_LineGenerate(p4.x, p4.y, (float)ADSR_WIDTH, 0.0f, x);
    }
    // 5. 默认/越界情况
    else
    {
        return 0.0f;
    }
}

ADSR ADSR1;