#include "../System/Common/Common.h"

#include "./ADSR.h"
#include "../System/Knob/Knob.h"
// #include "../System/Interface/Lumina_Interface.h" // Redundant include; ADSR.h already provides required declarations

/* ---- Externs ---- */
extern float ADC_BufferProcessed[4];

void ADSR::ADSR_Init(){
    if(_mediator1 != NULL){
        _mediator1->ParamMediator_Subscribe(ParamTopics::ADC_C6_Param, this);
        _mediator1->ParamMediator_Subscribe(ParamTopics::ADC_C7_Param, this);
        _mediator1->ParamMediator_Subscribe(ParamTopics::ADC_C14_Param, this);
        _mediator1->ParamMediator_Subscribe(ParamTopics::ADC_C15_Param, this);
    }
}

/**
 * @brief 单线段函数解析式与求值 (浮点数版本)
 * @param x1 起始点横坐标
 * @param y1 起始点纵坐标
 * @param x2 终止点横坐标
 * @param y2 终止点纵坐标
 * @param x  当前查询的横坐标
 * @return 对应的纵坐标函数值 (float)
 */
float ADSR::ADSR_LineGenerate(float x1, float y1, float x2, float y2, float x) {
    // 1. 边界检查：若 x 在区间外，直接返回临界值
    if (x <= x1) return y1;
    if (x >= x2) return y2;

    // 2. 计算横轴差值，防止除以 0 导致的崩溃
    float Deltax = x2 - x1;
    if (Deltax == 0.0f) return y1;

    // 3. 计算纵轴差值
    float Deltay = y2 - y1;

    // 4. 线性插值计算：y = y1 + k * (x - x1)
    float y = y1 + (Deltay * (x - x1)) / Deltax;

    return y;
}

/**
 * @brief 根据给定点集和x坐标实时生成对应函数值 (浮点数版本)
 * @param p2 第一拐点 (Attack 结束, Decay 开始)
 * @param p3 第二拐点 (Decay 结束, Release 开始)
 * @param p4 第三拐点 (Release 结束)
 * @param x  当前的横坐标 (时间轴)
 * @return 对应的纵坐标函数值 (幅度)
 */
float ADSR::ADSR_FunctionValueCalculate(float x) {  
    // 1. Attack 阶段：从原点 (0,0) 到 points[0]
    if(x >= 0.0f && x < this->points[0].x)
    {
        return this->ADSR_LineGenerate(0.0f, 0.0f, this->points[0].x, this->points[0].y, x);
    }
    // 2. Decay 阶段：从 points[0] 到 points[1]
    else if(x >= this->points[0].x && x < this->points[1].x)
    {
        return this->ADSR_LineGenerate(this->points[0].x, this->points[0].y, this->points[1].x, this->points[1].y, x);
    }
    // 3. Sustain 阶段：从 points[1] 到 points[2]
    else if(x >= this->points[1].x && x < this->points[2].x)
    {
        return this->ADSR_LineGenerate(this->points[1].x, this->points[1].y, this->points[2].x, this->points[2].y, x);
    }
    // 4. Release 阶段：从 points[2] 到 屏幕终点 (向 0 归集)
    else if(x >= this->points[2].x && x < (float)ADSR_WIDTH)
    {
        return this->ADSR_LineGenerate(this->points[2].x, this->points[2].y, (float)ADSR_WIDTH, 0.0f, x);
    }
    // 5. 默认/越界情况
    else
    {
        return 0.0f;
    }
}

void ADSR::ADSR_StepCalculate(void){
    // 依据 ADSR_Period 与 ADSR_FM_CONSTANT 计算每个采样点在横轴上的步进
    this->ADSR_Step = ADSR_FM_CONSTANT;
}

void ADSR::ADSR_BufferFill(uint8_t HalfFlag){
    float* Start = (HalfFlag == 0) ? &this->Buffer[0] : &this->Buffer[HALF_BUFFER_LENGTH];

    for(int i = 0 ; i < HALF_BUFFER_LENGTH ; i++){
        Start[i] = this->ADSR_FunctionValueCalculate(this->ADSR_Accmulation);
        this->ADSR_Accmulate();
    }

    if (HalfFlag) {
        this->ADSR_StepCalculate();
    }
}

void ADSR::ADSR_Accmulate(void){
    this->ADSR_Accmulation += this->ADSR_Step;
    if (this->ADSR_Accmulation >= (float)ADSR_WIDTH) {
        this->ADSR_Accmulation -= (float)ADSR_WIDTH;
    }
}