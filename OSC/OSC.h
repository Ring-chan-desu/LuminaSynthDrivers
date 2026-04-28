#ifndef __OSC_H__
#define __OSC_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "main.h"
#include "dac.h"

#include "../System/Interface/Lumina_Interface.h"
#include "../System/Mediator/Mediator.h"
#include "../System/Knob/Knob.h"
#include "../WaveForm/WaveForm.h"

#include <stdint.h>



#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */
class OSC : public Subscriber {
    private: // 参数名m待修改
        Mediator* m = 0;

    public:
        // 1. 冒号后面是【初始化列表】，只负责给变量赋值
        OSC(Mediator* Med)
        :   m(Med),
            WaveForm((uint16_t*)WaveFormList[0]),
            FM_Coeff(0),
            AM_Coeff(&ADC_BufferProcessed[1]),
            Index(0),
            PhaseFlag(0),
            Accmulation(0.0f),
            TargetFreq(144.0f),
            ActualFreq(this->TargetFreq),
            Step(9.386667f)
        {
            if (m != nullptr) {
                m->Mediator_Subscribe(Topics::OSC_FM, this); // 在这里订阅！
            }
        }

        uint16_t* WaveForm;
        uint16_t Buffer[FULL_BUFFER_LENGTH];

        uint16_t FM_Coeff;
        uint16_t* AM_Coeff;

        uint16_t Index;
        uint8_t PhaseFlag;

        float Accmulation;
        float TargetFreq;
        float ActualFreq;
        float Step;

        void Subscriber_Update(Topics t, float value) override{
            if (t == Topics::OSC_FM) {
                this->FM_Coeff = (uint16_t)value;
            }
        }

        void OSC_Init(void);
        
        void OSC_BufferFill(uint8_t HalfFlag);
        void OSC_Accmulate(void);
        void OSC_StepCalculate(void);

        void OSC_WaveFormSelect(uint8_t WaveFormIndex);

        void OSC_FM(uint16_t Max);
        float OSC_AM(uint16_t Max);
        uint16_t OSC_Lerp(void); // 线性插值
};

/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */


/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
extern uint16_t *OutBuffer;
// void OSCGeneralInit();

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __OSC_H__ */