#ifndef __MEDIATOR_H__
#define __MEDIATOR_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "main.h"
#include "etl/map.h"
#include "etl/vector.h"

#include <stdint.h>


/* ---- 2. C/C++ 通用类型声明 ---- */


#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
/* ---- 频道枚举 ----*/
enum class Topics : uint8_t // 指定底层类型
{
    OSC_FM = 0,     // 振荡器调频
    // OSC_AM,         // 振荡器调幅
};

class Subscriber
{
    public:
        virtual void Subscriber_Update(Topics topic, float value) = 0;
};

class Mediator
{
    private:
        etl::map<Topics, etl::vector<Subscriber*, 2>, 2> SubscribersMap;
        Mediator() {}   //  单例一定要有私有的构造函数,不然会报错
    public:
        static Mediator& GetInstance() {    //  麦耶斯单例
            static Mediator Instance;   //  调用这个函数的时候才开始初始化,用的就是类私有的构造函数在内存中预留空间给实例
            return Instance;
            //  返回当前实例引用,可以通过这个函数访问,比如说 Mediator::GetInstance().Attribute = value;
        }

        void Mediator_Subscribe(Topics topic, Subscriber* subscriber){
            SubscribersMap[topic].push_back(subscriber);
        } // 订阅即注册频道

        // void Mediator_Unsubscribe(Topics topic, Subscriber* subscriber){
        //     auto it = SubscribersMap.find(topic);   // 先找找看
        //     if (it != SubscribersMap.end()) {   //  看看有没有这个话题
        //         it->second.remove(subscriber);
        //     }
        // }

        void Mediator_Publish(Topics topic, float value) {
            auto it = SubscribersMap.find(topic); // 先找找看
            if (it != SubscribersMap.end()) {     // 如果有人订阅
                for (Subscriber* s : it->second) {
                    s->Subscriber_Update(topic, value);
                }
            }
        }

        Mediator(const Mediator&) = delete; //  禁用拷贝构造函数
        Mediator& operator = (const Mediator&) = delete;    //  禁用赋值操作符
};

extern Mediator MediatorTest;
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __MEDIATOR_H__ */