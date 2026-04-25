#include "stm32f407xx.h"
#include "tim.h"

/* ---- Target Libraries ---- */
#include "../System/Mediator/Mediator.h"
#include "../System/Knob/Knob.h"

#include "./CallbackHook.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3; 

void CallbackHooks(){
    Mediator::GetInstance().Mediator_Publish(Topics::OSC_FM, ADC_BufferProcessed[3]);  //  Knobs Publish    
}