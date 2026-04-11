/* ----Modules & Semaphores Files Includes---- */
#include "ADSR.h"
#include "adsrMatrix.h"
#include "button.h"

/* ----Modules & Semaphores Bridge Functions---- */
void Bridge_Init(void)
{
    /* From "test" */
	// TaskInit();
    Test_Button_TaskInit();

    /*From "adsrMatrix"*/
    /*Tasks*/
    adsrMatrix_DMAEnable_TaskInit();
    /*Sems*/
    adsrMatrix_DisplayPeriod_SemInit();
    adsrMatrix_DMAComplete_SemInit();

    /*Form "ADSR"*/
    /*Tasks*/
    ADSR_ADCPulse_TaskInit();
    ADSR_DrawEnvelopeTracer_TaskInit();
    /*Sems*/
    ADSR_MIDI_ON_STATE_SemInit();
    ADSR_MIDI_OFF_STATE_SemInit();
}