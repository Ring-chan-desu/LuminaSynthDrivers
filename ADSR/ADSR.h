#ifndef __ADSR_H__
#define __ADSR_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "../System/Interface/Lumina_Interface.h"
#include "../Mediator/Mediator.h"
#include "../System/Interface/Lumina_Interface.h"

/* ---- 2. C/C++ 通用类型声明 ---- */
typedef struct {
    float x;
    float y;
}ADSR_Point;    //  点结构体

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
class ADSR : public Subscriber {
    private:
        Mediator* _mediator = 0;

    public:
        ADSR(Mediator* mediator)
        :   _mediator(mediator),
            points{
                {0.0f, ADSR_HEIGHT},                // p2: Attack 结束点
                {ADSR_WIDTH / 4.0f, ADSR_HEIGHT},   // p3: Decay 结束点
                {ADSR_WIDTH / 4.0f, 0.0f}           // p4: Release 开始点
            }, // 这里注释对于点的描述有点问题,嘛...至少我是这么觉得啦
                // 正在听工作陪伴音声所以可能会有些低质量发言...这条注释就是最好的证明.
            a(&points[0].x),
            d(&points[1].x),
            s(&points[1].y),
            r(&points[2].x)
        {

        }
        ADSR_Point points[ADSR_POINT_COUNT];    //  起始点和终止点略去

        float* a;
        float* d;
        float* s;
        float* r;

        float ADSR_Accmulation = 0.0f;
        float ADSR_Step = 0.1f;

        void ADSR_Init(void);
        float ADSR_LineGenerate(float x1, float y1, float x2, float y2, float x);
        float ADSR_FunctionValueCalculate(float x);
        // void ADSR_Accmulate(void);   //  未完成

        void Subscriber_Update(Topics t, float value) override {
            // 此时 value 已经是经过死区处理且归一化的权重 (0.0f - 1.0f)
            if (t == Topics::ADC_C6) {  // Attack
                *a = value * ADSR_WIDTH;
            }
            if (t == Topics::ADC_C7) {  // Decay
                // 保证 d 坐标始终在 a 之后：d = a + 权重 * 剩余宽度
                *d = *a + (value * (ADSR_WIDTH - *a));
            }
            if (t == Topics::ADC_C14) { // Sustain
                *s = value * ADSR_HEIGHT;
            }
            if (t == Topics::ADC_C15) { // Release
                // 保证 r 坐标始终在 d 之后：r = d + 权重 * 剩余宽度
                *r = *d + (value * (ADSR_WIDTH - *d));
            }
        }
};

// extern ADSR ADSR1(&(Mediator::GetInstance()));
extern ADSR ADSR1;
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
/* ---- RTOS ---- */
void ADSR_Update_TaskInit(void);
void ADSR_Update_SemInit(void);

extern osSemaphoreId_t ADSR_Update_SemHandle;
#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __ADSR_H__ */

/*
2026.4.28 总结
    本来说给旋钮全连接到ADSR上但是没实现,中途遇到了新的问题
    就是关于Mediator Update方法的实现问题,其会尝试遍历所有Subscribers的update方法,放在中断里执行还是太勉强了
    这之后考虑用RTOS任务和Queue替代 当然不是现在,要不然就是过早优化了
    キラキラ～
*/