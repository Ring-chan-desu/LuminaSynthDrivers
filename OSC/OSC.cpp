#include "OSC.h"

#include "../System/Common/Common.h"

// #include "./OSC.h" // Redundant duplicate include
// #include "../System/Interface/Lumina_Interface.h" // Redundant include; OSC.h and Common.h provide required declarations
#include "../System/Mediator/Mediators.h"   //  中转站
#include "../WaveForm/WaveForm.h"   //  波表
#include "../System/Knob/Knob.h"
#include "arm_math.h"
#include <stdint.h>
#include "../MIDI/midi-in/midiIn.h" //  MIDI

float oscGeneralOutBuffer[HALF_BUFFER_LENGTH] = {0};  //  所有OSC唯一共用的缓冲区

/* ---- 振荡器 ---- */

void OSC::OSC_StepCalculate(void){  //  一次性全算完
    for (commonParam& instance : oscSlot) {
        if (instance.gate == true) {
            instance.step = instance.targetFreq * FM_CONSTANT;
        }
    }
}

void OSC::OSC_Accmulate(void){  // 统一累加
    for (commonParam& instance : oscSlot) {
        instance.accmulation += instance.step;
        if (instance.accmulation >= (float)WAVEFORM_LENGTH) {
            instance.accmulation -= (float)WAVEFORM_LENGTH;
        }
    }
}

void OSC::OSC_calculate(void){  
    arm_fill_f32(0.0f, oscGeneralOutBuffer, HALF_BUFFER_LENGTH);
    uint8_t activeCount = 0;
    float weight = 0.0f;
    for (commonParam& instance : oscSlot) {
        // if (instance.gate == false) {
        //     continue;
        // } else {
            activeCount++;
        // }
        float step = instance.step;
        for (int i = 0; i < HALF_BUFFER_LENGTH; i++) {
            oscGeneralOutBuffer[i] += this->OSC_Lerp(instance, step);
        }
    }
    if (activeCount == 0) {
        return; //  退出函数
    }
    weight = 1.0f / (float)activeCount;
    arm_scale_f32(oscGeneralOutBuffer, weight, oscGeneralOutBuffer, HALF_BUFFER_LENGTH);
}

uint16_t OSC::OSC_Lerp(commonParam& instance, float step){   //  本方法集成了累加器取值和波表取值,最终输出的是根据当前累加器取波表的结果值
    uint16_t index_l = (uint16_t)instance.accmulation;

    uint16_t index_r = index_l + 1;
    if (index_r >= WAVEFORM_LENGTH) {
        index_r = 0;
    }

    float frac = instance.accmulation - (float)index_l;

    float y0 = (float)this->WaveForm[index_l];  //  波表是共用的
    float y1 = (float)this->WaveForm[index_r];

    uint32_t result = (uint32_t)(y0 + frac * (y1 - y0));
    instance.accmulation += step;
    if (instance.accmulation >= (float)WAVEFORM_LENGTH) {
        instance.accmulation -= (float)WAVEFORM_LENGTH;
    }  //  线性插值之后的自动累加
    return (uint16_t)(result > 4095 ? 4095 : result); // 强制限幅
}

void OSC::OSC_update(void){
    this->OSC_midiRead();
    this->OSC_StepCalculate();
}

void OSC::OSC_midiRead(void){
    if (pQueue.empty()) {
        return; //  空队列直接不看了,拜拜了您内
    }
    commonParam* instance = nullptr;    //  用于存放被找到的元素的指针
    if (pQueue.front().isNoteOnEvent) { //  如果是noteOn
        bool isFind = false;    //  标志位先置0
        for (int i = 0; i < MAX_POLY_NUM; i++) {    //  先遍历看有无空闲的
            instance = &this->oscSlot[i]; //  每到一个元素更新一次指针
            if (instance->gate == false) {   //  发现空闲的就赋值

                instance->targetFreq = pQueue.front().Freq;
                instance->timestamp = pQueue.front().timestamp;
                instance->gate = true;   //  赋值
                // instance->note = pQueue.front().note;
                instance->Freq = pQueue.front().Freq;
                pQueue.pop();

                isFind = true;  //  找到了
                break;  //  跳出当前循环
            }
        }

        if (!isFind) {  //  没找到空闲的就找时间戳最大的然后直接切掉
            instance = &this->oscSlot[0];    //  先给0号元素提取出来
            for (int i = 1; i < MAX_POLY_NUM; i++) {    //  依次检查其他元素
                if (this->oscSlot[i].timestamp > instance->timestamp) {    //  如果当前元素的时间戳比前面的时间戳大,那么就丢到擂台上当擂主
                    instance = &this->oscSlot[i]; //  更新指针
                }
            }
            //  最后指针所指的就是时间戳最大的那个元素
            instance->gate = true;
            instance->targetFreq = pQueue.front().Freq;
            instance->timestamp = pQueue.front().timestamp;   //  赋值
            // instance->note = pQueue.front().note;
            instance->Freq = pQueue.front().Freq;
            pQueue.pop();
        }
    } else {    // 下面是释放逻辑
        bool isEmpty = true;    //  擂主空
        
        for (int i = 0; i < MAX_POLY_NUM; i++) {
            // 如果当前通道满足"活着且同音"
            // if (this->oscSlot[i].gate == true && this->oscSlot[i].note == pQueue.front().note) {
            if (this->oscSlot[i].gate == true && this->oscSlot[i].Freq == pQueue.front().Freq) {    //  音名判断改为频率判断
                
                if (isEmpty == false) { // 擂台上已经有初始擂主了,开始对比时间戳
                    if (this->oscSlot[i].timestamp > instance->timestamp) { 
                        instance = &this->oscSlot[i];  // 找到更大的，替换擂主
                    }
                } else {
                    instance = &this->oscSlot[i];
                    isEmpty = false;
                }
            }
        }
        if (isEmpty == false) { // 只有真正找到了同音通道,才执行关闭
            instance->gate = false; 
            instance->accmulation = 0.0f;
            instance->step = 0.0f;
            instance->targetFreq = 0.0f;
            instance->actualFreq = 0.0f;
            instance->timestamp = 0; 
            // instance->note = 0xFFFF;    
            instance->Freq = 0.0f;
        }
        pQueue.pop(); // 无论找没找到,处理完当前事件后,队列都推前一个
    }
}