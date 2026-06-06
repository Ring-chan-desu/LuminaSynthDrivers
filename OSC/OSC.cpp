#include "OSC.h"

#include "../System/Common/Common.h"

// #include "./OSC.h" // Redundant duplicate include
// #include "../System/Interface/Lumina_Interface.h" // Redundant include; OSC.h and Common.h provide required declarations
#include "../System/Mediator/Mediators.h"   //  中转站
#include "../WaveForm/WaveForm.h"   //  波表
#include "../System/Knob/Knob.h"
#include "stm32f4xx_hal_adc.h"
#include <stdint.h>
#include "../MIDI/midi-in/midiIn.h" //  MIDI

/* ---- 振荡器 ---- */

void OSC::OSC_StepCalculate(void){  //  一次性全算完
    // this->OSC_FM(CAPTURE_UPPER_LIMIT);
    // this->Step = this->ActualFreq * FM_CONSTANT;
    for (commonParam& instance : oscSlot) {
        if (instance.gate == true) {
            instance.step = instance.targetFreq * FM_CONSTANT;
        }
    }
}

void OSC::OSC_Accmulate(void){  // 统一累加
    // this->Accmulation += this->Step;
    // if (this->Accmulation >= (float)WAVEFORM_LENGTH) {
    //     this->Accmulation -= (float)WAVEFORM_LENGTH;
    // }
    for (commonParam& instance : oscSlot) {
        instance.accmulation += instance.step;
        if (instance.accmulation >= (float)WAVEFORM_LENGTH) {
            instance.accmulation -= (float)WAVEFORM_LENGTH;
        }
    }
}

//  TODO: 记得改个名字
//  TODO: 最终输出幅值和,加权$\frac{1}{16}$
void OSC::OSC_calculate(void){  
    for (int i = 0; i < HALF_BUFFER_LENGTH; i++) {
        float sum = 0;
        int activeCount = 0; // 记录到底有几个通道在响
        for (commonParam& instance : oscSlot) {
            if (instance.gate == true) { // 🌟 只有开门的通道才准加进来！
                sum += this->OSC_Lerp(instance);
                activeCount++;
            }
        }
        this->OSC_Accmulate();
        if (activeCount == 0){
            this->oscBuffer[i] = 2048;
        } else {
            this->oscBuffer[i] = sum / (float)activeCount;
        }
    }
}

uint16_t OSC::OSC_Lerp(commonParam& instance){   //  本方法集成了累加器取值和波表取值,最终输出的是根据当前累加器取波表的结果值
    uint16_t index_l = (uint16_t)instance.accmulation;

    uint16_t index_r = index_l + 1;
    if (index_r >= WAVEFORM_LENGTH) {
        index_r = 0;
    }

    float frac = instance.accmulation - (float)index_l;

    float y0 = (float)this->WaveForm[index_l];  //  波表是共用的
    float y1 = (float)this->WaveForm[index_r];

    uint32_t result = (uint32_t)(y0 + frac * (y1 - y0));
    return (uint16_t)(result > 4095 ? 4095 : result); // 强制限幅
}

//  TODO: 现在频率不是公用的了,这个也得重写
void OSC::OSC_update(void){
    // this->TargetFreq = currentNote.Freq;    //  频率
    this->OSC_midiRead();
    this->OSC_StepCalculate();
    this->OSC_calculate();
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
                instance->note = pQueue.front().note;
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
            instance->note = pQueue.front().note;
            pQueue.pop();
        }
    } else {    //  下面是释放逻辑
        for (int i = 0; i < MAX_POLY_NUM; i++) {    //  先遍历,找个同音的
            if (this->oscSlot[i].gate == true && this->oscSlot[i].note == pQueue.front().note) {    //  同音判断
                instance = &this->oscSlot[i];   //  随便找个同音的当擂主,把它指针传给instance
                break;
            }
        }
        for (int i = 0; i < MAX_POLY_NUM; i++) {    //  再遍历一遍,找同音的跟擂主比较
            if (this->oscSlot[i].gate == true && this->oscSlot[i].note == pQueue.front().note) {
                if (this->oscSlot[i].timestamp > instance->timestamp) {
                    instance = &this->oscSlot[i];  //  找到更大的放在擂台上
                }
            }
        }
        // 最后剩下来的就是最老的同音了,把他关上
        instance->gate = false; //  先关闭
        instance->accmulation = 0.0f;
        instance->step = 0.0f;
        instance->targetFreq = 0.0f;
        instance->actualFreq = 0.0f;
        instance->timestamp = 0; //  参数全清零
        instance->note = 0xFFFF;    //  不可能匹配到的midi编号
        pQueue.pop();
    }
}