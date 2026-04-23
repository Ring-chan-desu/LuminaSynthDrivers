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
        etl::map<Topics, etl::vector<Subscriber*, 10>, 10> SubscribersMap;
    public:
        void Mediator_Subscribe(Topics topic, Subscriber* subscriber){
            SubscribersMap[topic].push_back(subscriber);
        } // 订阅即注册频道

        void Mediator_Publish(Topics topic, float value) {
            auto it = SubscribersMap.find(topic); // 先找找看
            if (it != SubscribersMap.end()) {     // 如果有人订阅
                for (Subscriber* s : it->second) {
                    s->Subscriber_Update(topic, value);
                }
            }
        }
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