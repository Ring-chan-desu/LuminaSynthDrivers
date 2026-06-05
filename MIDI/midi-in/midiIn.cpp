#include "./midiIn.h"
#include "usart.h"

uint8_t MIDI_receiveBuffer[MIDI_BUFFER_SIZE] = {0}; 
MIDI_stateMachine currentState = MIDI_stateMachine::IDLE;   
midiNote currentNote;   //  当前音符

// etl::list<midiNote, MIDI_POLY_SIZE> plist; //  midi复音缓冲区
etl::queue<midiNote, 128> pQueue;   //  复音队列

uint16_t writeIndex = 0;  
uint16_t readIndex  = 0;  

/*前置声明*/
void MIDI_stateMachineProcess(uint8_t data);    
void MIDI_midiInit();

/* ---- MIDI received Task ---- */
osThreadId_t MIDI_received_TaskHandle;

void MIDI_received_Task(void *argument)
{
    HAL_UART_Receive_DMA(&huart3, MIDI_receiveBuffer, MIDI_BUFFER_SIZE);
    MIDI_midiInit();
    for(;;)
    {
        osDelay(2); //  每隔 2ms 执行一次,process放在delay之后防止读到脏数据
        writeIndex = MIDI_BUFFER_SIZE - DMA1_Stream1->NDTR;   //  直接读寄存器
        while (readIndex != writeIndex) 
        {
            uint8_t rawData = MIDI_receiveBuffer[readIndex];    //  直接取数据

            MIDI_stateMachineProcess(rawData);  //  处理函数不需要取数据,处理数据即可

            readIndex = (readIndex + 1) % MIDI_BUFFER_SIZE; //  在处理方法外的回绕
        }
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

void MIDI_stateMachineProcess(uint8_t data)
{
    switch (currentState) {
        case MIDI_stateMachine::IDLE:
            if ((data & 0xF0) == 0x90) {    //  注意此处运算符优先级, == 比 & 高
                currentNote.isNoteOnEvent = true;   
                currentNote.channel = (data & 0x0F) + 1;
                currentState = MIDI_stateMachine::WAITING_NOTE; 
            } else if ((data & 0xF0) == 0x80) {
                currentNote.isNoteOnEvent = false;  
                currentNote.channel = data & 0x0F;
                currentState = MIDI_stateMachine::WAITING_NOTE; //  这两行看似重复,但是不能移到if外面去,不然在IDLE状态下无论何数据都会进入WAITING_NOTE
            }
            break;

        case MIDI_stateMachine::WAITING_NOTE:   
            currentNote.note = data;
            if (currentNote.isNoteOnEvent) {    
                currentState = MIDI_stateMachine::WAITING_VELOCITY; 
            } else {
                pQueue.push(currentNote);
                ParamMediator::Param_GetInstance().ParamMediator_Publish(ParamTopics::MIDI_isUpdate, 0);
                currentState = MIDI_stateMachine::IDLE; // 松开时序到此结束,我们可以在这里给中转站发消息
            }
            break;
        
        case MIDI_stateMachine::WAITING_VELOCITY:   //  按下的时序到此处结束,在这里调用OSC的方法
            currentNote.velocity = data;    
            currentNote.Freq = midiLUT[currentNote.note];
            currentNote.timestamp = HAL_GetTick();  //  时间戳
            pQueue.push(currentNote);
            ParamMediator::Param_GetInstance().ParamMediator_Publish(ParamTopics::MIDI_isUpdate, 0);
            currentState = MIDI_stateMachine::IDLE; 
            break;

        default:    
            currentState = MIDI_stateMachine::IDLE; 
            break;
    }
}

void MIDI_midiInit(){
    currentState = MIDI_stateMachine::IDLE;
    currentNote.isNoteOnEvent = false;
    readIndex = 0;
    writeIndex = 0;
}