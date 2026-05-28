#ifndef __PARAMMEDIATOR_H__
#define __PARAMMEDIATOR_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../Common/Common.h"
#include "etl/map.h"
#include "etl/vector.h"

/* ---- 2. C/C++ 通用类型声明 ---- */


#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
/* ---- 频道枚举 ----*/
enum class ParamTopics : uint8_t // 点单例话题
{
    ADC_C1 = 0,
    ADC_C2,
    ADC_C3,
    ADC_C4,
    ADC_C5,
    ADC_C6_Param,
    ADC_C7_Param,
    ADC_C8,
    ADC_C9,
    ADC_C10,
    ADC_C11,
    ADC_C12,
    ADC_C13,
    ADC_C14_Param,
    ADC_C15_Param,
};

class ParamSubscriber
{
    public:
        virtual void ParamSubscriber_Update(ParamTopics topic, float value) = 0;  //  订阅者被外部触发的Update方法
};


class ParamMediator
{
    private:
        etl::map<ParamTopics, etl::vector<ParamSubscriber*, 10>, 10> SubscribersMap;  //  流单例的数据类型要变化,由数变为指针
        ParamMediator() {}   //  实现单例一定要有私有的构造函数,不然会报错
    public:
        static ParamMediator& Param_GetInstance() {    //  点单例
            static ParamMediator Param_Instance;   //  调用这个函数的时候才开始初始化,用的就是类私有的构造函数在内存中预留空间给实例
            return Param_Instance;
            //  返回当前实例引用,可以通过这个函数访问,比如说 Mediator::GetInstance().<Attribute> = <value>;
        }

        void ParamMediator_Subscribe(ParamTopics topic, ParamSubscriber* subscriber){
            SubscribersMap[topic].push_back(subscriber);
        } // 订阅即注册频道

        void ParamMediator_Unsubscribe(ParamTopics topic, ParamSubscriber* subscriber){
            auto it = SubscribersMap.find(topic);
            if (it != SubscribersMap.end()) {
                auto& vec = it->second; //  这里second是etl那边的属性,代表键值对的值
                for (auto vIt = vec.begin(); vIt != vec.end(); ++vIt) {
                    if (*vIt == subscriber) {
                        vec.erase(vIt); //  从etl::vector中删除
                        break;
                    }
                }
            }
        } // 取消订阅频道

        void ParamMediator_Publish(ParamTopics topic, float value) {  //  点单例的单对单发布
            auto it = SubscribersMap.find(topic); // 先找找看
            if (it != SubscribersMap.end()) {     // 如果有人订阅 如果找不到会返回end
                for (ParamSubscriber* s : it->second) {
                    s->ParamSubscriber_Update(topic, value);
                }
            }
        }   //   发布

        /* 你问我流单例呢?流单例我们只需要在中断回调那边遍历流单例的注册表然后实现向量化加权就好,而点单例需要考虑的就多了 */
        // 对于各个使用中转站对象的实例化,嘛...因为现在单例变多了所以实例化那边也要做出相应的调整

        ParamMediator(const ParamMediator&) = delete; //  禁用拷贝构造函数
        ParamMediator& operator = (const ParamMediator&) = delete;    //  禁用赋值操作符
};


/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __PARAMMEDIATOR_H__ */