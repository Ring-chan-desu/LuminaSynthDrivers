#ifndef __OSC_H__
#define __OSC_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "../System/Common/Common.h"
#include "ParamMediator.h"
#include "dac.h"

// #include "main.h" // Moved to Common.h
// #include "../System/Interface/Lumina_Interface.h" // Redundant dependency; common includes are centralized in Common.h
#include "../System/Mediator/Mediators.h"
#include "../System/Knob/Knob.h"
#include "../WaveForm/WaveForm.h"
#include "../MIDI/midi-in/midiIn.h"
#include "etl/queue.h"
#include <stdint.h>

// #include <stdint.h> // Moved to Common.h

// #define SAMPLE_RATE             48000                               // 采样率
// #define HALF_BUFFER_LENGTH      256                                 // 半缓冲区长度
// #define FULL_BUFFER_LENGTH      512   

#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */
class commonParam{
    public:
    commonParam()
    :
        accmulation(0.0f),
        step(0.0f),
        targetFreq(440.0f), //  440Hz默认频率
        actualFreq(0.0f),
        Freq(0.0f),
        timestamp(0)
    {}

    float accmulation;
    float step;
    float targetFreq;
    float actualFreq;
    float Freq;
    uint32_t timestamp;
    bool gate;
};

class OSC : public ParamSubscriber {
    private: // 参数名m待修改
        ParamMediator* m1 = 0;
        StreamMediator* m2 = 0;

    public:
        /* ---- 初始化列表 ---- */
        OSC(ParamMediator* Med1, StreamMediator* Med2)
        :   m1(Med1),
            m2(Med2),
            WaveForm((uint16_t*)WaveFormList[0]),
            FM_Coeff(0.0f),
            AM_Coeff(1.0f),
            PhaseFlag(0)
        {
            if (m1 != nullptr) {

            }

            if (m2 != nullptr) {

            }
        }
        /* ---- 属性 ---- */
        uint16_t* WaveForm;
        float FM_Coeff;
        float AM_Coeff;
        uint8_t PhaseFlag;
        commonParam oscSlot[MAX_POLY_NUM];

        /* ---- 方法 ---- */
        void ParamSubscriber_Update(ParamTopics t, float value) override{   //  继承自 ParamSubscriber,考虑弃用

        }
        void OSC_Init(void);
        
        void OSC_BufferFill(uint8_t HalfFlag);

        
        void OSC_StepCalculate(void);
        void OSC_Accmulate(void);
        void OSC_calculate(void);
        uint16_t OSC_Lerp(commonParam& instance, float step); // 线性插值
        void OSC_update(void);
        void OSC_midiRead(void);

        void OSC_WaveFormSelect(uint8_t WaveFormIndex); //  待完善

        void OSC_FM(uint16_t Max);
        float OSC_AM(uint16_t Max); 
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* --- 3. C/C++ 通用类型声明 --- */

/* --- 4. 供 C 调用的公开 API (Wrapper) --- */
extern float oscGeneralOutBuffer[HALF_BUFFER_LENGTH];
// void OSCGeneralInit();

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __OSC_H__ */