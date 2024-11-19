#include<iostream>
#include<hgl/type/object/ObjectRelation.h>

using namespace hgl;

class Base
{
    ObjectBaseInfo obj_base_info;                           ///<对象基本信息

    ObjectRelation obj_relation;                            ///<对象引用关系

public:

    const size_t GetTypeHash()const noexcept{return obj_base_info.hash_code;}

public:
};

template<typename T> class Inherit:public T
{
    size_t hash_code;

public:

    Inherit()
    {
        hash_code=GetTypeHash<Inherit>();
    }

    const size_t GetClassTypeHash()const noexcept{return hash_code;}
};

class TestA:public Inherit<Base>
{

public:

};

int main()
{
    TestA test;

    std::cout<<"Base1: "<<base1.StaticTypeHash()<<std::endl;
    std::cout<<"Base2: "<<base2.StaticTypeHash()<<std::endl;

    return 0;
}