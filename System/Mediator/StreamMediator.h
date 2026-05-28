#ifndef __PSTREAMEDIATOR_H__
#define __PSTREAMEDIATOR_H__

/* ---- 1. 通用头文件包含与宏定义 ---- */
#include "../Common/Common.h"
#include "etl/map.h"
#include "etl/vector.h"

/* ---- 2. C/C++ 通用类型声明 ---- */
enum class StreamTopics : uint8_t // 流单例话题
{
    LFO1_Stream = 0,
    ADSR1_Stream
};

#ifdef __cplusplus
/* ---- 3. 仅 C++ 可见的类定义 ---- */
class StreamSubscriber
{
    public:
        virtual void StreamSubscriber_Update(ParamTopics topic, float value) = 0;  //  订阅者被外部触发的Update方法
};

class StreamMediator
{
    private:
        etl::map<StreamTopics, etl::vector<StreamSubscriber*, 10>, 10> SubscribersMap;  //  流注册表
        //先遍历一遍话题找this,然后给这个话题的发布者的缓冲区指针加入订阅者自己的缓冲区
        
    public:
        static StreamMediator& Stream_GetInstance() {    //  流单例
            static StreamMediator Stream_Instance;
            return Stream_Instance;
            //  返回当前实例引用,可以通过这个函数访问,比如说 Mediator::GetInstance().<Attribute> = <value>;
        }
};
/* 开启 C 兼容接口定义 */
extern "C" {
#endif

/* ---- 4. 供 C 调用的公开 API (Wrapper) ---- */


#ifdef __cplusplus
} // 结束 extern "C"
#endif

#endif /* __STREAMMEDIATOR_H__ */