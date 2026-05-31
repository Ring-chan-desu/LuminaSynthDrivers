#ifndef __OSC_H__
#define __OSC_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "../System/Common/Common.h"
#include "dac.h"

// #include "main.h" // Moved to Common.h
// #include "../System/Interface/Lumina_Interface.h" // Redundant dependency; common includes are centralized in Common.h
#include "../System/Mediator/Mediators.h"
#include "../System/Knob/Knob.h"
#include "../WaveForm/WaveForm.h"
#include <stdint.h>

// #include <stdint.h> // Moved to Common.h

// #define SAMPLE_RATE             48000                               // 采样率
// #define HALF_BUFFER_LENGTH      256                                 // 半缓冲区长度
// #define FULL_BUFFER_LENGTH      512   

#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */
typedef struct{
    float accmulation;
    float step;
    float targetFreq;
    float actualFreq;
    bool gate;
}commonParam;

class OSC : public ParamSubscriber {
    private: // 参数名m待修改
        ParamMediator* m1 = 0;
        StreamMediator* m2 = 0;

    public:
        // 1. 冒号后面是【初始化列表】，只负责给变量赋值
        OSC(ParamMediator* Med1, StreamMediator* Med2)
        :   m1(Med1),
            m2(Med2),
            WaveForm((uint16_t*)WaveFormList[0]),
            FM_Coeff(0.0f),
            AM_Coeff(1.0f),
            PhaseFlag(0)
            // gate(0),
            // Accmulation(0.0f),
            // TargetFreq(440.0f),
            // ActualFreq(this->TargetFreq),
            // Step(9.386667f)
        {
            if (m1 != nullptr) {
                // m1->ParamMediator_Subscribe(ParamTopics::ADC_C6_Param, this); // 在这里订阅！
                // m1->ParamMediator_Subscribe(ParamTopics::ADC_C7_Param, this); // 在这里订阅！
                // m1->ParamParamMediator_Subscribe(ParamTopics::LFO1, this);
            }

            if (m2 != nullptr) {

            }
        }

        uint16_t* WaveForm;
        // float Buffer[FULL_BUFFER_LENGTH];    //  OSC缓冲区,现在不需要了

        float FM_Coeff;
        float AM_Coeff;

        uint8_t PhaseFlag;
        commonParam oscSlot[MAX_POLY_NUM];
        // uint8_t gate;

        // float Accmulation;
        // float TargetFreq;
        // float ActualFreq;
        // float Step;

        void ParamSubscriber_Update(ParamTopics t, float value) override{
            if (t == ParamTopics::ADC_C6_Param) {  //  FM
                this->FM_Coeff = value;
            }
            if (t == ParamTopics::ADC_C7_Param) {  //  AM
                this->AM_Coeff = value;
            }
            // if (t == ParamTopics::LFO1) {
            //     this->AM_Coeff *= value;
            // }
        }

        void OSC_Init(void);
        
        void OSC_BufferFill(uint8_t HalfFlag);
        void OSC_update(void);
        uint16_t OSC_calculate(void);
        void OSC_Accmulate(void);
        void OSC_StepCalculate(void);

        void OSC_WaveFormSelect(uint8_t WaveFormIndex); //  待完善

        void OSC_FM(uint16_t Max);
        float OSC_AM(uint16_t Max);
        uint16_t OSC_Lerp(void); // 线性插值
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */

// void OSCGeneralInit();

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __OSC_H__ */