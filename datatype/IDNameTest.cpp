#include<hgl/type/IDName.h>
#include<iostream>

void main()
{
    using namespace hgl;

    UTF8IDName id1("id1");
    UTF8IDName id2("id2");

    AnsiIDName id3("id1");
    AnsiIDName id4("id2");

    UTF8IDName id5("id1");
    AnsiIDName id6("id1");

    std::cout<<id1.GetID()<<":"<<id1.GetName()<<", ClassID: "<<std::hex<<id1.GetClassID()<<std::endl;
    std::cout<<id2.GetID()<<":"<<id2.GetName()<<", ClassID: "<<std::hex<<id2.GetClassID()<<std::endl;

    std::cout<<id3.GetID()<<":"<<id3.GetName()<<", ClassID: "<<std::hex<<id3.GetClassID()<<std::endl;
    std::cout<<id4.GetID()<<":"<<id4.GetName()<<", ClassID: "<<std::hex<<id4.GetClassID()<<std::endl;

    if(id1==id5)
        std::cout<<"id1==id5"<<std::endl;
    else
        std::cout<<"id1!=id5"<<std::endl;
}
