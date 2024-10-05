#include <iostream>

#include<hgl/type/object/Object.h>

using namespace hgl;

class DebugObject:public Object
{
    HGL_OBJECT_CLASS_BODY(DebugObject)

public:

    void Initailize()
    {
        std::cout<<"DebugObject::Initailize("<<GetSerialNumber()<<")"<<std::endl;
    }

    void Deinitailize() override
    {
        std::cout<<"DebugObject::Deinitailize("<<GetSerialNumber()<<")"<<std::endl;
    }
};

#include<hgl/type/object/ObjectManager.h>

HGL_DEFINE_DEFAULT_OBJECT_MANAGER(DebugObject);

#include<hgl/type/object/DefaultCreateObject.h>

int main()
{
    SafePtr<DebugObject> obj1=HGL_NEW_OBJECT(DebugObject);

    HGL_DEFINE_OBJECT(DebugObject,obj2);                    //等于上一行

//    DebugObject *obj3=new DebugObject();                  //编译不过(构造函数被定义为私有)

    obj1.Release();

    //delete obj2;                                          //编译不过,SafePtr<>不能被delete

    if(obj1.IsValid())
    {
        std::cerr<<"[ERROR] obj1 IsValid() error!"<<std::endl;
    }
    else
    {
        std::cout<<"[ OK ] obj1 isn't valid!"<<std::endl;
    }

    const size_t obj2_sn=obj2->GetSerialNumber();

    SafePtr<DebugObject> obj2_indirect=GetObjectBySerial<DebugObject>(obj2_sn);           //直接根据序列号获取对象

    if(!obj2_indirect.IsValid())
    {
        std::cerr<<"[ERROR] obj2_indirect isn't valid!"<<std::endl;                         //获取失败(不应该)
    }
    else if(obj2_indirect!=obj2)
    {
        std::cerr<<"[ERROR] obj2_indirect!=obj2"<<std::endl;                                //获取的对象不是obj2(不应该)
    }
    else
    {
        std::cout<<"[ OK ] obj2_indirect==obj2"<<std::endl;                                //获取的对象是obj2(成功)

        obj2_indirect.Destory();

        if(obj2.IsValid())
        {
            std::cerr<<"[ERROR] obj2 IsValid() error!"<<std::endl;                          //错误，由于obj2_indirect被Destory了,obj2也应该无效
        }
        else
        {
            std::cout<<"[ OK ] obj2 isn't valid!"<<std::endl;                              //成功
        }
    }

    return 0;
}
