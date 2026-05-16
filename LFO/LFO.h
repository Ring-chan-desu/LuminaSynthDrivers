#ifndef __LFO_H__
#define __LFO_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "../System/Common/Common.h"

// #include "FreeRTOS.h" // Moved to Common.h
// #include "cmsis_os2.h" // Moved to Common.h
// #include "task.h" // Moved to Common.h
// #include "main.h" // Moved to Common.h
// #include "cmsis_os.h" // Moved to Common.h

#include "../System/Mediator/Mediator.h"
#include "../WaveForm/WaveForm.h"

// #include <stdint.h> // Moved to Common.h

#define LFO_WEIGHT_MAX 1.0f

#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */
class LFO : public Subscriber {
    private: // 参数名m待修改
        Mediator* m = 0;
        uint8_t prescalerCount = 0;

    public:
        // 1. 冒号后面是【初始化列表】，只负责给变量赋值
        LFO(Mediator* Med)
        :   m(Med),
            WaveForm((uint16_t*)WaveFormList[0]),
            Accmulation(0.0f),
            TargetFreq(5.0f),  // LFO 低频，默认 5Hz
            ActualFreq(TargetFreq),
            Step(0)  // FM_CONSTANT
        {
            if (m != nullptr) {
                m->Mediator_Subscribe(Topics::ADC_C6, this); // 订阅频率控制
            }
        }

        uint16_t* WaveForm;

        float Accmulation;
        float TargetFreq;
        float ActualFreq;
        float Step;

        void Subscriber_Update(Topics t, float value) override{
            if (t == Topics::ADC_C6) {  // 频率控制
                this->TargetFreq = value * 10.0f;  // 假设 value 是 0-1，映射到 0-10Hz
            }
        }

        void LFO_Update(void);
        void LFO_Accmulate(void);
        void LFO_StepCalculate(void);
        void LFO_GetValue(void);

        uint16_t LFO_Lerp(void); // 线性插值
        void LFO_FM(uint16_t Max);
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
void LFO_Update_SemInit(void);
void LFO_Update_TaskInit(void);

extern osSemaphoreId_t LFO_Update_SemHandle;
// void LFOGeneralInit();

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __LFO_H__ */