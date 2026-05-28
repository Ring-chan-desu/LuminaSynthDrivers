#ifndef __ADSR_H__
#define __ADSR_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../System/Common/Common.h"

// #include "FreeRTOS.h" // Moved to Common.h
// #include "cmsis_os2.h" // Moved to Common.h
// #include "task.h" // Moved to Common.h
// #include "main.h" // Moved to Common.h
// #include "cmsis_os.h" // Moved to Common.h

// #include "../System/Interface/Lumina_Interface.h" // Redundant include; common dependencies are centralized in Common.h
#include "../System/Mediator/Mediators.h"
// #include "../System/Interface/Lumina_Interface.h" // Duplicate and redundant

// #define ADSR_POINT_COUNT    3          //  ADSR有效点数
// #define ADSR_HEIGHT         100.0f     //  ADSR高度
// #define ADSR_WIDTH          100.0f     //  ADSR宽度

/* ---- 2. C/C++ 通用类型声明 ---- */
// class Mediator;

typedef struct {
    float x;
    float y;
}ADSR_Point;    //  点结构体

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
class ADSR : public ParamSubscriber {
    private:
        ParamMediator* _mediator1 = 0;
        StreamMediator* _mediator2 = 0;

    public:
        ADSR(ParamMediator* mediator1, StreamMediator* mediator2)
        :   _mediator1(mediator1),
            _mediator2(mediator2),
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
        float Buffer[FULL_BUFFER_LENGTH];    // ADSR 缓冲区

        uint8_t PhaseFlag = 0;               // 半缓冲区标志，参考 OSC 实现风格

        void ADSR_Init(void);
        float ADSR_LineGenerate(float x1, float y1, float x2, float y2, float x);
        float ADSR_FunctionValueCalculate(float x);
        void ADSR_Accmulate(void);
        void ADSR_StepCalculate(void);
        void ADSR_BufferFill(uint8_t HalfFlag);

        void ParamSubscriber_Update(ParamTopics t, float value) override {
            // 此时 value 已经是经过死区处理且归一化的权重 (0.0f - 1.0f)
            if (t == ParamTopics::ADC_C6_Param) {  // Attack
                *a = value * ADSR_WIDTH;
            }
            if (t == ParamTopics::ADC_C7_Param) {  // Decay
                // 保证 d 坐标始终在 a 之后：d = a + 权重 * 剩余宽度
                *d = *a + (value * (ADSR_WIDTH - *a));
            }
            if (t == ParamTopics::ADC_C14_Param) { // Sustain
                *s = value * ADSR_HEIGHT;
            }
            if (t == ParamTopics::ADC_C15_Param) { // Release
                // 保证 r 坐标始终在 d 之后：r = d + 权重 * 剩余宽度
                *r = *d + (value * (ADSR_WIDTH - *d));
            }
        }
};

// extern ADSR ADSR1(&(Mediator::Param_GetInstance()));
extern ADSR ADSR1;
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */

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