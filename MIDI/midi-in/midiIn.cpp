#include "./midiIn.h"
#include "usart.h"
#include <stdalign.h>

uint8_t MIDI_receiveBuffer[MIDI_BUFFER_SIZE] = {0}; //  串口接收缓冲区
MIDI_stateMachine currentState = MIDI_stateMachine::IDLE;   
midiNote currentNote;   //  当前音符

// etl::list<midiNote, MIDI_POLY_SIZE> plist; //  midi复音缓冲区
etl::queue<midiNote, MIDI_BUFFER_SIZE> pQueue;   //  指令队列

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
        // osDelay(2); //  每隔 2ms 执行一次,process放在delay之后防止读到脏数据?
        writeIndex = MIDI_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart3.hdmarx);
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
        .priority = (osPriority_t) osPriorityBelowNormal7, // 提高一个优先级
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
                currentNote.Freq = midiLUT[currentNote.note];
                currentNote.timestamp = HAL_GetTick();
                pQueue.push(currentNote);
                // ParamMediator::Param_GetInstance().ParamMediator_Publish(ParamTopics::MIDI_isUpdate, 0);
                currentState = MIDI_stateMachine::IDLE; // 松开时序到此结束,我们可以在这里给中转站发消息
            }
            break;
        
        case MIDI_stateMachine::WAITING_VELOCITY:   //  按下的时序到此处结束,在这里调用OSC的方法
            currentNote.velocity = data;    
            currentNote.Freq = midiLUT[currentNote.note];
            currentNote.timestamp = HAL_GetTick();  //  时间戳
            pQueue.push(currentNote);
            // ParamMediator::Param_GetInstance().ParamMediator_Publish(ParamTopics::MIDI_isUpdate, 0);
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

void voiceAllocator::voiceAllocator_midiRead(){ //  由MIDI主动消费
    if (pQueue.empty()) {
        return; //  空队列直接不看了,拜拜了您内
    }
    commonParam* instance = nullptr;    //  用于存放被找到的元素的指针
    if (pQueue.front().isNoteOnEvent) { //  如果是noteOn
        bool isFind = false;    //  标志位先置0
        for (int i = 0; i < MAX_POLY_NUM; i++) {    //  先遍历看有无空闲的
            instance = &this->paramGroup[i]; //  每到一个元素更新一次指针
            if (instance->gate == false) {   //  发现空闲的就赋值
                instance->targetFreq = pQueue.front().Freq;
                instance->timestamp = pQueue.front().timestamp;
                instance->gate = true;   //  赋值
                pQueue.pop();

                isFind = true;  //  找到了
                break;  //  跳出当前循环
            }
        }

        if (!isFind) {  //  没找到空闲的就找时间戳最小的然后直接切掉
            instance = &this->paramGroup[0];    //  先给0号元素提取出来
            for (int i = 1; i < MAX_POLY_NUM; i++) {    //  依次检查其他元素
                if (this->paramGroup[i].timestamp < instance->timestamp) {    //  如果当前元素的时间戳比前面的时间戳小,那么就丢到擂台上当擂主
                    instance = &this->paramGroup[i]; //  更新指针
                }
            }
            //  最后指针所指的就是时间戳最小的那个元素
            instance->targetFreq = pQueue.front().Freq;
            instance->timestamp = pQueue.front().timestamp;
            instance->gate = true;   //  赋值
            pQueue.pop();
        }

    } else {    // 下面是释放逻辑
        bool isEmpty = true;    //  擂主空
        for (int i = 0; i < MAX_POLY_NUM; i++) {
            // 如果当前通道满足"活着且同音"
            if (this->paramGroup[i].gate == true && this->paramGroup[i].targetFreq == pQueue.front().Freq) {    //  音名判断改为频率判断
                
                if (isEmpty == false) { // 擂台上已经有初始擂主了,开始对比时间戳
                    if (this->paramGroup[i].timestamp < instance->timestamp) { 
                        instance = &this->paramGroup[i];  // 找到更大的，替换擂主
                    }
                } else {
                    instance = &this->paramGroup[i];
                    isEmpty = false;
                }
            }
        }

        if (isEmpty == false) { // 只有真正找到了同音通道,才执行关闭
            instance->gate = false;     //  先关掉通道
            // instance->step = 0.0f;
            instance->targetFreq = 0.0f;
            instance->actualFreq = 0.0f;
            instance->timestamp = 0; 
        }
        pQueue.pop(); // 无论找没找到,处理完当前事件后,队列都推前一个
    }
}
