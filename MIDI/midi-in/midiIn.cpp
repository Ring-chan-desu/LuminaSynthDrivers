#include "./midiIn.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "stm32f103xb.h"
#include "usart.h"
#include <stdint.h>
#include <stdbool.h>

uint8_t MIDI_receiveBuffer[MIDI_BUFFER_SIZE] = {0}; 
MIDI_stateMachine currentState = MIDI_stateMachine::IDLE;   
static midiNote currentNote;       
static bool isNoteOnEvent = true;   

int debug_trigger = 0; 

// ✨ 下标统一改成标准的 uint16_t，防止算力溢出，初始化为 0
uint16_t writeIndex = 0;  
uint16_t readIndex  = 0;  

void MIDI_stateMachineProcess(uint8_t data);    

/* ---- MIDI received Task ---- */
osThreadId_t MIDI_received_TaskHandle;

void MIDI_received_Task(void *argument)
{
    // 🚀 开机第一件事，拉开 128 字节循环接收大网
    HAL_UART_Receive_DMA(&huart3, MIDI_receiveBuffer, MIDI_BUFFER_SIZE);

    for(;;)
    {
        osDelay(2); //  每隔 2ms 执行一次,process放在delay之后防止读到脏数据
        writeIndex = MIDI_BUFFER_SIZE - DMA1_Channel3->CNDTR;   //  直接读寄存器
        while (readIndex != writeIndex) 
        {
            uint8_t rawData = MIDI_receiveBuffer[readIndex];    //  直接取数据

            MIDI_stateMachineProcess(rawData);  //  处理函数不需要取数据,处理数据即可

            readIndex = (readIndex + 1) % MIDI_BUFFER_SIZE; //  在处理方法外的回绕
        }
    }
}

void MIDI_stateMachineProcess(uint8_t data)
{
    switch (currentState) {
        case MIDI_stateMachine::IDLE:
            if ((data & 0xF0) == 0x90) {    //  注意此处运算符优先级, == 比 & 高
                isNoteOnEvent = true;   
                currentNote.channel = data & 0x0F;
                currentState = MIDI_stateMachine::WAITING_NOTE; 
            } else if ((data & 0xF0) == 0x80) {
                isNoteOnEvent = false;  
                currentNote.channel = data & 0x0F;
                currentState = MIDI_stateMachine::WAITING_NOTE; 
            }
            break;

        case MIDI_stateMachine::WAITING_NOTE:   
            currentNote.note = data;
            if (isNoteOnEvent) {    
                currentState = MIDI_stateMachine::WAITING_VELOCITY; 
            } else {
                // <TODO>:松开后的处理
                currentState = MIDI_stateMachine::IDLE; 
            }
            break;
        
        case MIDI_stateMachine::WAITING_VELOCITY:   
            currentNote.velocity = data;    
            debug_trigger = 1;  //  用于调试
            currentState = MIDI_stateMachine::IDLE; 
            break;

        default:    
            currentState = MIDI_stateMachine::IDLE; 
            break;
    }
}

void MIDI_received_TaskInit(void)
{
    const osThreadAttr_t MIDI_received_TaskAttr = {
        .name = "MIDI_received_Task",
        .stack_size = 256 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };
    MIDI_received_TaskHandle = osThreadNew(MIDI_received_Task, NULL, &MIDI_received_TaskAttr);
}

void MIDI_midiInit(){
    currentState = MIDI_stateMachine::IDLE;
    isNoteOnEvent = false;
    readIndex = 0;
    writeIndex = 0;
}

void MIDI_rtosInit(){
    MIDI_received_TaskInit();
}