#ifndef __OSC_H__
#define __OSC_H__

/* --- 1. 通用头文件包含与宏定义 --- */
#include "main.h"
#include <stdint.h>

#define SAMPLE_RATE             48000           // 采样率
#define WAVEFORM_LENGTH         1024            // 波表长度
#define HALF_BUFFER_LENGTH      256             // 半缓冲区长度
#define FULL_BUFFER_LENGTH      512             // 全缓冲区长度
#define FM_CONSTANT             0.021333333f    // 调频常量 (1024 / 48000)

#ifdef __cplusplus
/* --- 2. 仅 C++ 可见的类定义 --- */
class OSC{
    public:
        uint16_t *WaveForm;
        uint16_t Buffer[FULL_BUFFER_LENGTH];

        uint16_t* FM_Coeff;
        uint16_t* AM_Coeff;

        uint16_t Index;
        uint8_t PhaseFlag;

        float Accmulation;
        float TargetFreq;
        float ActualFreq;
        float Step;

        void OSC_Init(void);
        void OSC_BufferFill(uint8_t HalfFlag);
        void OSC_Accmulate(void);
        void OSC_StepCalculate(void);
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
void OSCGeneralInit();

#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __OSC_H__ */