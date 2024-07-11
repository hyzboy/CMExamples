#include<hgl/type/IDName.h>
#include<iostream>

void main()
{
    using namespace hgl;

    UTF8IDName id1("utf8_1");
    UTF8IDName id2("utf8_2");

    AnsiIDName id3("ansi_1");
    AnsiIDName id4("ansi_2");

    std::cout<<id1.GetID()<<":"<<id1.GetName()<<", ClassID: "<<std::hex<<id1.GetClassID()<<std::endl;
    std::cout<<id2.GetID()<<":"<<id2.GetName()<<", ClassID: "<<std::hex<<id2.GetClassID()<<std::endl;

    std::cout<<id3.GetID()<<":"<<id3.GetName()<<", ClassID: "<<std::hex<<id3.GetClassID()<<std::endl;
    std::cout<<id4.GetID()<<":"<<id4.GetName()<<", ClassID: "<<std::hex<<id4.GetClassID()<<std::endl;
}
