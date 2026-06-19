#include "./Lumina_Interface.h"
#include "../OSC/OSC.h"
#include "../System/Mediator/Mediators.h"
#include "../ADSR/ADSR.h"
#include "../LFO/LFO.h"

/* ---- 实例化 ---- */

OSC OSC1(&(ParamMediator::Param_GetInstance()), nullptr);
// OSC OSC2(&(ParamMediator::Param_GetInstance()), nullptr);

LFO LFO1(&(ParamMediator::Param_GetInstance()), nullptr);

void Lumina_Interface_Init(void) {
    ParamMediator::Param_GetInstance();
}