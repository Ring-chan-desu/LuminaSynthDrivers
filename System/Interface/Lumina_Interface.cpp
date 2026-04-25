#include "./Lumina_Interface.h"
#include "../OSC/OSC.h"
#include "../System/Mediator/Mediator.h"

#include <stdint.h>
// #include "../Knob/Knob.h"

void Lumina_Interface_Init(void) {
    Mediator::GetInstance();
    // static OSC OSC1(&(Mediator::GetInstance()));
    OSCGeneralInit();
}

uint32_t* Lumina_Interface_GetOutBuffer(void) {
    return (uint32_t*)OutBuffer;
}