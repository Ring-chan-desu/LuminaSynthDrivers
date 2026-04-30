#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "stm32f407xx.h"
#include "tim.h"

/* ---- Target Libraries ---- */
#include "../System/Mediator/Mediator.h"
#include "../System/Knob/Knob.h"
#include "../ADSR/ADSR.h"

#include "./CallbackHook.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3; 

void CallbackHooks(){
    osSemaphoreRelease(ADSR_Update_SemHandle);  //  后续可以放到屏幕刷新
}