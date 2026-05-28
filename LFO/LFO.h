#ifndef __LFO_H__
#define __LFO_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "../System/Common/Common.h"

// #include "FreeRTOS.h" // Moved to Common.h
// #include "cmsis_os2.h" // Moved to Common.h
// #include "task.h" // Moved to Common.h
// #include "main.h" // Moved to Common.h
// #include "cmsis_os.h" // Moved to Common.h

#include "../System/Mediator/Mediators.h"
#include "../WaveForm/WaveForm.h"

// #include <stdint.h> // Moved to Common.h

#define LFO_WEIGHT_MAX 1.0f

#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */
class LFO : public ParamSubscriber {
    private: // 参数名m待修改
        ParamMediator* m1 = 0;
        StreamMediator* m2 = 0;
        uint8_t prescalerCount = 0;

    public:
        // 1. 冒号后面是【初始化列表】，只负责给变量赋值
        LFO(ParamMediator* Med1, StreamMediator* Med2)
        :   m1(Med1),
            m2(Med2),
            WaveForm((uint16_t*)WaveFormList[0]),
            FM_Coeff(1.0f),
            AM_Coeff(1.0f),
            Accmulation(0.0f),
            TargetFreq(1.0f),  // LFO 低频，默认 1Hz
            ActualFreq(this->TargetFreq),
            Step(0)  // FM_CONSTANT
        {
            if (m1 != nullptr) {
                m1->ParamMediator_Subscribe(ParamTopics::ADC_C6_Param, this); // 订阅频率控制
            }

            if (m2 != nullptr) {
                // m2->RegisterStream(StreamTopics::LFO1_Stream, this->Buffer);    //  订阅LFO的流
            }
        }

        uint16_t* WaveForm;
        float Buffer[FULL_BUFFER_LENGTH];    //  LFO缓冲区 此处用浮点

        float FM_Coeff;
        float AM_Coeff;
        
        uint8_t PhaseFlag;

        float Accmulation;
        float TargetFreq;
        float ActualFreq;
        float Step;

        void ParamSubscriber_Update(ParamTopics t, float value) override{
            if (t == ParamTopics::ADC_C6_Param) {  // 频率控制
                this->TargetFreq = value * 10.0f;  // 假设 value 是 0-1，映射到 0-10Hz
            }
        }

        void LFO_BufferFill(uint8_t HalfFlag);

        void LFO_Accmulate(void);
        void LFO_StepCalculate(void);
        void LFO_GetValue(void);

        uint16_t LFO_Lerp(void); // 线性插值
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __LFO_H__ */