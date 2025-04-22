#include<hgl/type/IDName.h>
#include<iostream>

int main(int,char **)
{
    using namespace hgl;

    AIDName id3("id1");
    AIDName id4("id2");

    AIDName id5("id1");
    AIDName id6("id2");

    std::cout<<id5.GetID()<<":"<<id5.GetName()<<", ClassID: "<<std::hex<<id5.GetClassID()<<std::endl;
    std::cout<<id6.GetID()<<":"<<id6.GetName()<<", ClassID: "<<std::hex<<id6.GetClassID()<<std::endl;

    std::cout<<id3.GetID()<<":"<<id3.GetName()<<", ClassID: "<<std::hex<<id3.GetClassID()<<std::endl;
    std::cout<<id4.GetID()<<":"<<id4.GetName()<<", ClassID: "<<std::hex<<id4.GetClassID()<<std::endl;

    if(id3==id5)
        std::cout<<"id3==id5"<<std::endl;
    else
        std::cout<<"id3!=id5"<<std::endl;
}
