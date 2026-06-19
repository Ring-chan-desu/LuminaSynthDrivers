#ifndef __MIDIIN_H__
#define __MIDIIN_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../../System/Common/Common.h"
#include "../../System/Mediator/ParamMediator.h"
#include "etl/queue.h"
#include <cstdint>

#define MIDI_BUFFER_SIZE 128    //  midi缓冲区大小
/* ---- 2. C/C++ 通用类型声明 ---- */


#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
enum class MIDI_stateMachine{
    IDLE = 0,
    WAITING_NOTE,
    WAITING_VELOCITY,
};

class commonParam{  //  这里借用了OSC的通用参数对象
    public:
    commonParam()
    :
        step(0.0f),
        targetFreq(440.0f), //  440Hz默认频率
        actualFreq(0.0f),
        gate(false),
        timestamp(0)
    {}

    float step;
    float targetFreq;
    float actualFreq;
    bool gate;
    uint32_t timestamp;
};

class midiNote
{
    public:
        midiNote()
        :   
            note(0),
            velocity(0),
            channel(0),
            isNoteOnEvent(false),
            Freq(0.0f),
            timestamp(0)
            {}

        uint8_t     note;
        uint8_t     velocity;
        uint8_t     channel;
        bool        isNoteOnEvent;
        float       Freq;
        uint32_t    timestamp;
};

class voiceAllocator
{
    private:

    public:
        voiceAllocator() {}

        commonParam paramGroup[MAX_POLY_NUM];

        void voiceAllocator_midiRead();
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
extern uint8_t MIDI_receiveBuffer[MIDI_BUFFER_SIZE];    //  midi接收缓冲区
extern etl::queue<midiNote, 128> pQueue;   //  复音队列
extern const float midiLUT[128];
extern midiNote currentNote;   //  当前音符

void MIDI_received_TaskInit(void);
void MIDI_midiInit(void);

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __MIDIIN_H__ */
