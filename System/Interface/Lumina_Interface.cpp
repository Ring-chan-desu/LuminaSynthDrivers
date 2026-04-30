#include "./Lumina_Interface.h"
#include "../OSC/OSC.h"
#include "../System/Mediator/Mediator.h"
#include "../ADSR/ADSR.h"

#include <stdint.h>
// #include "../Knob/Knob.h"

extern ADSR ADSR1;

void Lumina_Interface_Init(void) {
    Mediator::GetInstance();
    ADSR1.ADSR_Init();
    // static ADSR ADSR1(&(Mediator::GetInstance()));
    // static OSC OSC1(&(Mediator::GetInstance()));
    // OSCGeneralInit();
}

uint32_t* Lumina_Interface_GetOutBuffer(void) {
    return (uint32_t*)OutBuffer;
}

float Lumina_Interface_ADSR_GetValue(float CurrentX) {
    return ADSR1.ADSR_FunctionValueCalculate(CurrentX);
}