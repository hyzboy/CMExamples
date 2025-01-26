#include<typeinfo>
#include<iostream>
#include<hgl/type/object/Object.h>

class Object
{
public:

    virtual size_t GetTypeHash()const noexcept=0;
    virtual const char *GetTypeName()const noexcept=0;
};

template<typename T,typename BASE> class Inherit:public BASE
{
public:

    static const size_t StaticHash()noexcept
    {
        return typeid(T).hash_code();
    }

    virtual size_t GetTypeHash()const noexcept override
    {
        return T::StaticHash();
    }

    static const char *StaticName()noexcept
    {
        return typeid(T).name();
    }

    virtual const char *GetTypeName()const noexcept override
    {
        return T::StaticName();
    }
};//template<typename T> class Inherit:public T

class TestClass:public Inherit<TestClass,Object>
{
};

class TestClassA:public Inherit<TestClassA,TestClass>
{
};

class TestClassB:public Inherit<TestClassB,TestClass>
{
};

template<typename T> void out(const char *name,T *obj)
{
    std::cout<<name<<" static  "<<T::StaticName()<<": "<<std::hex<<T::StaticHash()<<std::endl;

    std::cout<<name<<" dynamic "<<obj->GetTypeName()<<": "<<std::hex<<obj->GetTypeHash()<<std::endl;
}

int main(int,char **)
{
    TestClass tc;
    TestClassA ta;
    TestClassB tb;

    out("tc",&tc);
    out("ta",&ta);
    out("tb",&tb);

    out<TestClass>("ta",&ta);
    out<TestClass>("tb",&tb);

    return 0;
}
