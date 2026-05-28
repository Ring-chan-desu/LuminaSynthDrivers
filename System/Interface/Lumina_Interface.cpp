#include "./Lumina_Interface.h"
#include "../OSC/OSC.h"
#include "../System/Mediator/Mediators.h"
#include "../ADSR/ADSR.h"
#include "../LFO/LFO.h"

// #include <stdint.h> // Moved to Common.h
// #include "../Knob/Knob.h"

// extern uint32_t *OutBuffer;
extern ADSR ADSR1;
/* ---- 实例化 ---- */
// OSC OSC1(&(ParamMediator::GetInstance()), nullptr); // updated to current ParamMediator singleton naming
OSC OSC1(&(ParamMediator::Param_GetInstance()), nullptr);
// LFO LFO1(&(ParamMediator::GetInstance()), nullptr); // updated to current ParamMediator singleton naming
LFO LFO1(&(ParamMediator::Param_GetInstance()), nullptr);
// ADSR ADSR1(&(ParamMediator::GetInstance()), nullptr); // updated to current ParamMediator singleton naming
ADSR ADSR1(&(ParamMediator::Param_GetInstance()), nullptr);

void Lumina_Interface_Init(void) {
    // ParamMediator::GetInstance(); // updated to current ParamMediator singleton naming
    ParamMediator::Param_GetInstance();
    // StreamMediator::GetInstance();
    ADSR1.ADSR_Init();

    // static ADSR ADSR1(&(ParamMediator::Param_GetInstance()));
    // static OSC OSC1(&(ParamMediator::Param_GetInstance()));
    // OSCGeneralInit();
}

extern "C" float* Lumina_Interface_GetOutBuffer(void) {
    return &(OSC1.Buffer[0]);   //  临时值
}

float Lumina_Interface_ADSR_GetValue(float CurrentX) {
    return ADSR1.ADSR_FunctionValueCalculate(CurrentX);
}