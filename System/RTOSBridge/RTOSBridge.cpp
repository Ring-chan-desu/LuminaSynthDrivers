#include "./RTOSBridge.h" // General Include

/* ----Modules & Semaphores Files Includes---- */
#include "../ADSR/ADSR.h"

/* ----Modules & Semaphores Bridge Functions---- */
void Bridge_Init(void)
{
    /* ---- From ADSR ---- */
    ADSR_Update_SemInit();
    ADSR_Update_TaskInit();
    
}