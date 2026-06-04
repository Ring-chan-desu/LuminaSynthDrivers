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
    uint32_t timestamp;
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
            // this->oscSlot[15] = {0.0f, 0.0f, 440.00f, 0.0f, true};
            if (m1 != nullptr) {
                // m1->ParamMediator_Subscribe(ParamTopics::ADC_C6_Param, this); // 在这里订阅！
                // m1->ParamMediator_Subscribe(ParamTopics::ADC_C7_Param, this); // 在这里订阅！
                // m1->ParamParamMediator_Subscribe(ParamTopics::LFO1, this);
                m1->ParamMediator_Subscribe(ParamTopics::MIDI_isUpdate, this);  //  订阅midi的更新频道
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
        // commonParam oscSlot[MAX_POLY_NUM] = {   //  频率这块硬编码先写死
            // {0.0f, 0.0f, 440.00f, 0.0f, 0, true},
            // {0.0f, 0.0f, 261.63f, 0.0f, 0, true}, // 根音 (Root)：中央C (C4) -> MIDI 编号 60
            // {0.0f, 0.0f, 329.63f, 0.0f, 0, true}, // 三音 (Third)：大三度 E4  -> MIDI 编号 64
            // {0.0f, 0.0f, 392.00f, 0.0f, 0, true}, // 五音 (Fifth)：纯五度 G4  -> MIDI 编号 67
        // };
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
            if (t == ParamTopics::MIDI_isUpdate) {
                commonParam* instance = nullptr;    //  用于存放被找到的元素的指针
                if (currentNote.isNoteOnEvent) {
                    bool isFind = false;    //  标志位先置0
                    for (int i = 0; i < MAX_POLY_NUM; i++) {    //  先遍历看有无空闲的
                        instance = &oscSlot[i]; //  每到一个元素更新一次指针
                        if (instance->gate == false) {   //  发现空闲的就赋值

                            instance->targetFreq = currentNote.Freq;
                            instance->timestamp = currentNote.timestamp;
                            instance->gate = true;   //  赋值

                            isFind = true;  //  找到了
                            break;  //  跳出当前循环
                        }
                    }

                    if (!isFind) {  //  没找到空闲的就找时间戳最大的然后直接切掉
                        instance = &oscSlot[0];    //  先给0号元素提取出来
                        for (int i = 1; i < MAX_POLY_NUM; i++) {    //  依次检查其他元素
                            if (oscSlot[i].timestamp > instance->timestamp) {    //  如果当前元素的时间戳比前面的时间戳大,那么就丢到擂台上当擂主
                                instance = &oscSlot[i]; //  更新指针
                            }
                        }
                        //  最后指针所指的就是时间戳最大的那个元素
                        instance->gate = true;
                        instance->targetFreq = currentNote.Freq;
                        instance->timestamp = currentNote.timestamp;   //  赋值
                    }
                } else {
                    for (int i = 0; i < MAX_POLY_NUM; i++) {    //  先遍历,找个同音的
                        instance = &oscSlot[i]; //  依旧更新指针
                        if (oscSlot[i].gate == true && oscSlot[i].targetFreq == currentNote.Freq) {    //  同音判断
                            instance = &oscSlot[i];   //  随便找个同音的当擂主,把它指针传给instance
                            break;
                        }
                    }
                    for (int i = 0; i < MAX_POLY_NUM; i++) {    //  再遍历一遍,找同音的跟擂主比较
                        if (oscSlot[i].timestamp > instance->timestamp) {
                            instance = &oscSlot[i];  //  找到更大的放在擂台上
                        }
                    }
                    // 最后剩下来的就是最老的同音了,把他关上
                    instance->gate = false; //  先关闭
                    instance->accmulation = 0.0f;
                    instance->step = 0.0f;
                    instance->targetFreq = 0.0f;
                    instance->actualFreq = 0.0f;
                    instance->timestamp = 0; //  参数全清零
                }
            }
        }
        void OSC_Init(void);
        
        void OSC_BufferFill(uint8_t HalfFlag);

        
        void OSC_StepCalculate(void);
        void OSC_Accmulate(void);
        uint16_t OSC_calculate(void);
        uint16_t OSC_Lerp(commonParam& instance); // 线性插值
        void OSC_update(void);

        void OSC_WaveFormSelect(uint8_t WaveFormIndex); //  待完善

        void OSC_FM(uint16_t Max);
        float OSC_AM(uint16_t Max); 
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