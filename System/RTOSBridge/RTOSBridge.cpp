#include "./RTOSBridge.h" // General Include

#include "../Common/Common.h"   //  各种各样不变的依赖

/* ----Modules & Semaphores Files Includes---- */
#include "../System/Knob/Knob.h"
#include "../LFO/LFO.h"

/* ----Modules & Semaphores Bridge Functions---- */
void Bridge_Init(void)
{
    /* ---- From Knob ---- */
    // Knobs_Update_SemInit();
    // Knobs_Update_TaskInit();
    // General_Interrupt_TaskInit();
    // General_Interrupt_SemInit();
}