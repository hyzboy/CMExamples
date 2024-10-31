/**
* 资源管理器
* 
* 资源管理器并非一个简单的模块，而是一个成套的组件。
* 
* 由一个统一的管理器管理所有的资源，并提供多种索引获取方式，对资源同时提供引用计数。
* 
* 同时，对引用和释放可以产生详细的日志供开发者调试分析。
*/

#include<iostream>
#include<hgl/type/object/ObjectBaseInfo.h>

using namespace hgl;
using namespace std;

class IGraphResObject
{
    ObjectSimpleInfo    osi;

    int ref_count;

public:

    const size_t GetTypeHash()const noexcept{return osi.hash_code;}         ///<获取数据类型的HASH值
    const size_t GetSerial()const noexcept{return osi.serial_number;}       ///<获取数据序列号

    const int GetRefCount()const noexcept{return ref_count;}

public:

    IGraphResObject(const ObjectSimpleInfo &info)
    {
        osi=info;
        ref_count=0;

        cout<<"type("<<info.hash_code<<") serial("<<info.serial_number<<") create."<<endl;
    }

    virtual ~IGraphResObject()
    {
        cout<<"type("<<osi.hash_code<<") serial("<<osi.serial_number<<") destroy."<<endl;
    }
};//class IGraphResObject

template<typename T> class GraphResObject:public IGraphResObject
{
    static size_t SerialCount=0;

public:

    GraphResObject():IGraphResObject({typeid(T).hash_code,SerialCount++}){}
    virtual ~GraphResObject()=default;
};//class GraphResObject

class IGraphResManager
{
    size_t SerialCount;

public:


};

class TestObjectA{};
class TestObjectB{};

using TOA=GraphResObject<TestObjectA>;
using TOB=GraphResObject<TestObjectB>;

int main()
{
    TOA *toa=new TOA;
    TOB *tob=new TOB;

    cout<<"toa->GetRefCount():"<<toa->GetRefCount()<<endl;
    cout<<"tob->GetRefCount():"<<tob->GetRefCount()<<endl;

    delete toa;
    delete tob;

    return(0);
}