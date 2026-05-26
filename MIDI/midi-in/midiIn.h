#ifndef __MIDIIN_H__
#define __MIDIIN_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../../System/Common/Common.h"

#define MIDI_BUFFER_SIZE 128
/* ---- 2. C/C++ 通用类型声明 ---- */


#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
//  状态待补充
enum class MIDI_stateMachine{
    IDLE = 0,
    WAITING_NOTE,
    WAITING_VELOCITY,
};

class midiNote
{
    public:
        uint8_t note;
        uint8_t velocity;
        uint8_t channel;
};

/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */
extern uint8_t MIDI_receiveBuffer[MIDI_BUFFER_SIZE];

void MIDI_rtosInit(void);
void MIDI_midiInit(void);

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __MIDIIN_H__ */
