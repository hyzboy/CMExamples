/**

对象关系测试

    1.每个对象有自己的 type_hash , 由typeid(T).hash_code()获取。
        同时含有唯一 serial number

    2.每个对象都有自己的引用计数和引用记录

    3.引用记录中，会记录当前对象被谁引用，以及引用代码的位置

    4.同时，引用记录中，会包含一个引用serial，用于标识当前引用的唯一性。
        在出现对象泄露时，可根据引用记录查询是那个对象忘记了释放引用。

*/

#include<hgl/type/object/Object.h>
#include<hgl/type/List.h>
#include<iostream>

using namespace hgl;
using namespace std;

#define CLASS_TYPE_HASH(type)   public: static const size_t StaticTypeHash(){return typeid(type).hash_code();}


class BaseObject
{
    size_t object_hash_code;
    size_t object_serial;

    CLASS_TYPE_HASH(BaseObject)

public:

    BaseObject(const size_t hash,const size_t os)
    {
        object_hash_code=hash;
        object_serial   =os;
    }

    virtual ~BaseObject()=default;
    
    const size_t GetTypeHash()const{return object_hash_code;}
    const size_t GetObjectSerial()const{return object_serial;}
};//class BaseObject

#define CLASS_BODY(class_type)  CLASS_TYPE_HASH(class_type)    \
                                private:    \
                                    static const size_t CreateObjectSerial(){static size_t serial=0;return ++serial;} \
                                public: \
                                    class_type():BaseObject(class_type::StaticTypeHash(),class_type::CreateObjectSerial()){}    \
                                    virtual ~class_type()=default;


template<typename T> inline const bool IsType(BaseObject *bo){return bo?(bo->GetTypeHash()==T::StaticTypeHash()):false;}

class TestObject:public BaseObject
{
    CLASS_BODY(TestObject)

public:

};

template<typename T> class SharedObject
{
    T *obj;

    int ref_count;
    int weak_count;

public:

    SharedObject()
    {
        obj=nullptr;
        ref_count=0;
        weak_count=0;
    }

    ~SharedObject()
    {
        if(obj)
            delete obj;
    }

    void SetObject(T *o)
    {
        if(obj)
            delete obj;

        obj=o;
    }

    T *GetObject()
    {
        return obj;
    }

    void AddRef()
    {
        ++ref_count;
    }

    void DelRef()
    {
        if(ref_count>0)
            --ref_count;
    }

    void AddWeak()
    {
        ++weak_count;
    }

    void DelWeak()
    {
        if(weak_count>0)
            --weak_count;
    }

    int GetRefCount()const
    {
        return ref_count;
    }
    int GetWeakCount()const
    {
        return weak_count;
    }
};

int main(int,char **)
{
    TestObject to1,to2,to3;

    BaseObject *bo=&to3;

    std::cout<<"TestObject::StaticHash:"<<TestObject::StaticTypeHash()<<std::endl;

    std::cout<<"to1.serial: "<<to1.GetObjectSerial()<<std::endl;
    std::cout<<"to2.serial: "<<to2.GetObjectSerial()<<std::endl;

    std::cout<<"bo->StaticHash: "<<bo->StaticTypeHash()<<std::endl;
    std::cout<<"bo->TypeHash:   "<<bo->GetTypeHash()<<std::endl;
    std::cout<<"bo->Serial:     "<<bo->GetObjectSerial()<<std::endl;

    const bool result=IsType<TestObject>(bo);
    
    std::cout<<"IsType<TestObject>(bo) result is "<<(result?"true":"false")<<std::endl;

    return 0;
}