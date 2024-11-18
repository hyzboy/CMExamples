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
#include<tsl/robin_set.h>
#include<iostream>

using namespace hgl;
using namespace std;

#define CLASS_TYPE_HASH(type)   static const size_t StaticTypeHash(){return typeid(type).hash_code();}

class BaseObject
{
    ObjectSimpleInfo object_simple_info;

public:

    BaseObject(const ObjectSimpleInfo &osi)
    {
        object_simple_info=osi;
    }

    virtual ~BaseObject()=default;

    CLASS_TYPE_HASH(BaseObject)

    const size_t GetTypeHash()const{return object_simple_info.hash_code;}
    const size_t GetObjectSerial()const{return object_simple_info.serial_number;}
};//class BaseObject

#define CLASS_BODY(class_type)  private:    \
                                    static const size_t CreateObjectSerial(){static size_t serial=0;return ++serial;} \
                                public: \
                                    class_type():BaseObject(ObjectSimpleInfo(class_type::StaticTypeHash(),class_type::CreateObjectSerial())){}    \
                                    virtual ~class_type()=default;  \
                                    CLASS_TYPE_HASH(class_type)


template<typename T> inline const bool IsType(BaseObject *bo){return bo?(bo->GetTypeHash()==T::StaticTypeHash()):false;}

inline const bool TypeEqual(BaseObject *bo1,BaseObject *bo2)
{
    if(!bo1||!bo2)return(false);

    return bo1->GetTypeHash()==bo2->GetTypeHash();
}

class TestObject:public BaseObject
{
    CLASS_BODY(TestObject)

public:

};

using ObjectSimpleInfoSet=tsl::robin_set<ObjectSimpleInfo>;

/**
* 数据指针引用<br>
* 类似于std::shared_ptr,SharedPtr。但这个类强大在于它会记录谁引用了它，以及它引用了谁。
*/
template<typename T> class RefPtr
{
    struct RefData
    {
        void *obj;

        ObjectSimpleInfoSet ref_set;
        ObjectSimpleInfoSet weak_set;
    };

    RefData *data;

public:

    RefPtr()
    {
        obj=nullptr;
        ref_count=0;
        weak_count=0;
    }

    ~RefPtr()
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

    bool result=IsType<TestObject>(bo);
    
    std::cout<<"IsType<TestObject>(bo) result is "<<(result?"true":"false")<<std::endl;

    result=TypeEqual(&to1,bo);

    std::cout<<"TypeEqual(&to1,bo) result is "<<(result?"true":"false")<<std::endl;

    return 0;
}