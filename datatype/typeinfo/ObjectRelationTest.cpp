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
    ObjectBaseInfo object_simple_info;

public:

    BaseObject(const ObjectBaseInfo &osi)
    {
        object_simple_info=osi;
    }

    virtual ~BaseObject()=default;

    CLASS_TYPE_HASH(BaseObject)

    const ObjectBaseInfo &GetObjectBaseInfo()const{return object_simple_info;}
    const size_t GetTypeHash()const{return object_simple_info.hash_code;}
    const size_t GetObjectSerial()const{return object_simple_info.unique_id;}

    virtual void Destory()
    {
        delete this;
    }
};//class BaseObject

#define CLASS_BODY(class_type)  private:    \
                                    static const size_t CreateObjectSerial(){static size_t serial=0;return ++serial;} \
                                public: \
                                    class_type():BaseObject(ObjectBaseInfo(class_type::StaticTypeHash(),class_type::CreateObjectSerial())){}    \
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

void test1()
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
}

using ObjectSimpleInfoSet=tsl::robin_set<ObjectBaseInfo>;

template<typename T> class RefPtr;

/**
* 引用对象
*/
template<typename T> class RefObject
{
    T *obj;

    ObjectSimpleInfoSet ref_me_set;     ///<引用我的对象
    ObjectSimpleInfoSet me_ref_set;     ///<我引用的对象

    template<typename T> friend class RefPtr;

public:

    RefObject(T *o)
    {
        obj=o;
    }

    ~RefObject()
    {
        if(obj)
            obj->Destory();
    }

    /**
    * 申请一个引用
    */
    RefPtr<T> Acquire(const ObjectBaseInfo *osi,const SourceCodeLocation &scl);

    void Release(RefPtr<T> *rp,const SourceCodeLocation &)
    {
        if(!rp)return;


    }
};//template<typename T> class RefObject

template<typename T> class RefPtr
{
    RefObject<T> *ref_obj;

public:

    RefPtr()
    {
        ref_obj=nullptr;
    }

    RefPtr(RefObject<T> *ro)
    {
        ref_obj=ro;
    }

    ~RefPtr()
    {
        if(ref_obj)
            ref_obj->Release(this,HGL_SCL_HERE);
    }

    bool IsValid()const
    {
        if(!this)return(false);
        if(!ref_obj)return(false);
        if(!ref_obj->obj)return(false);

        return(true);
    }

    operator T *()
    {
        return ref_obj->obj;
    }

    void Release(const SourceCodeLocation &scl)
    {
        if(!ref_obj)return;

        ref_obj->Release(this,scl);
    }
};

#define ACQUIRE_REF(ref_object,self) ref_object->Acquire(&self->GetObjectBaseInfo(),HGL_SCL_HERE);

#define REF_PTR_RELEASE(obj)    obj->Release(HGL_SCL_HERE);

class TestTexture:public BaseObject
{
    CLASS_BODY(TestTexture)
};

class TestMaterial:public BaseObject
{
    CLASS_BODY(TestMaterial)

    RefPtr<TestTexture> texture;

public:

    void Init(RefObject<TestTexture> *ref_tex)
    {
        texture=ACQUIRE_REF(ref_tex,this);
    }
};

void test2()
{
    RefObject<TestTexture> ref_tex1=new TestTexture;

    TestMaterial *mtl=new TestMaterial;

    mtl->Init(&ref_tex1);
}

int main(int,char **)
{


    return 0;
}