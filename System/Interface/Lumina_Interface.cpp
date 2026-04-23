#include "./Lumina_Interface.h"
#include "../OSC/OSC.h"
#include <stdint.h>
// #include "../Knob/Knob.h"

void Lumina_Interface_Init(void) {
    OSCGeneralInit();
}

uint32_t* Lumina_Interface_GetOutBuffer(void) {
    return (uint32_t*)OutBuffer;
}