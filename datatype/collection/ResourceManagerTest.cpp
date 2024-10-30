/**
* 资源管理器
* 
* 资源管理器并非一个简单的模块，而是一个成套的组件。
* 
* 由一个统一的管理器管理所有的资源，并提供多种索引获取方式，对资源同时提供引用计数。
* 
* 同时，对引用和释放可以产生详细的日志供开发者调试分析。
*/

#include<hgl/type/LifetimeCallback.h>
#include<hgl/type/object/ObjectBaseInfo.h>
#include<hgl/type/object/ObjectManager.h>
#include<hgl/type/List.h>

using namespace hgl;

struct RefRecord;
struct RefUser;
struct ResObject;

/**
* 引用记录
*/
struct RefRecord
{
    constexpr const static unsigned int ACTION_INC=1;     ///<增加引用
    constexpr const static unsigned int ACTION_DEC=2;     ///<减少引用

public:

    ObjectSimpleInfo user_osi;          ///<引用者对象简单信息

    uint action;                        ///<动作

    int new_ref_count;                  ///<新的引用计数
};

/**
* 引用者信息
*/
struct RefUser
{
    ObjectSimpleInfo user_osi;           ///<对象简单信息
};

/**
* 资源对象基类
*/
class ResObject:public ObjectSimpleInfo
{
    int ref_count;                      ///<引用计数

    List<RefRecord> ref_record_list;    ///<引用记录列表

public:

    const int GetRefCount()const noexcept{return ref_count;}

    /**
    * 增加引用计数
    */
    virtual void IncRef() noexcept
    {
        ++ref_count;
    }

public:
};//class ResObject

class ResManager:public ObjectManager
{
public:


};
