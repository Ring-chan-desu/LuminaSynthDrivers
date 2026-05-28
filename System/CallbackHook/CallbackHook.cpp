#include "../Common/Common.h"

// #include "FreeRTOS.h" // Moved to Common.h
// #include "cmsis_os2.h" // Moved to Common.h
// #include "task.h" // Moved to Common.h
// #include "main.h" // Moved to Common.h
// #include "cmsis_os.h" // Moved to Common.h

// #include "stm32f407xx.h" // Moved to Common.h
#include "tim.h"

/* ---- Target Libraries ---- */
#include "../System/Mediator/Mediators.h"
#include "../System/Knob/Knob.h"

#include "../ADSR/ADSR.h"
#include "../LFO/LFO.h"

#include "./CallbackHook.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3; 

void CallbackHooks(){
    osSemaphoreRelease(Knobs_Update_SemHandle);  //  ADSR信号量释放 后续可以放到屏幕刷新
    // test();
}