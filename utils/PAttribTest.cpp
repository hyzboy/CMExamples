#include<iostream>
#include<iomanip>
#include<hgl/util/plist/LoadPAttrib.h>
#include<hgl/util/plist/SavePAttrib.h>

using namespace hgl;
using namespace std;

int main(int argc,char **argv)
{
    if(argc!=3)
    {
        std::cout<< "PAttribTest use format:\t[cmd] [filename]"<<std::endl<<std::endl<<
                    "Example:"<<std::endl<<
                    "\tPAttribTest c 1.txt\tCreate a plist file."<<std::endl<<
                    "\tPAttribTest l 1.txt\tLoad a plist file."<<std::endl<<std::endl;

        return 0;
    }

    PAttribSet<char> pl_set;

    //                                              属性名称        缺省值         最小值     最大值    小数位数
    //--------------------------------------------+-----------+---------------+--------+----------+------
    auto *name  =pl_set.CreateStringAttrib(         "name",     "Anonymous");
    auto *sex   =pl_set.CreateBoolAttrib(           "sex",      true);
    auto *age   =pl_set.CreateNumberAttrib(         "age",      17,              1,         120);
    auto *temp  =pl_set.CreateFloatAttrib<float>(   "temp",     36.5,           35,         42,     2);

    const char cmd=argv[1][0];

    if(cmd=='c'||cmd=='C')
    {
        SaveToTextFile<char,ByteOrderMask::UTF8>(ToOSString(argv[2]),pl_set);
    }
    else
    {
        LoadFromTextFile<char>(ToOSString(argv[2]),pl_set);

        //lambda方式
        pl_set.Enum([](const String<char> &key,PAttribBase<char> *attr)
        {
            std::cout<<std::setw(8)<<key.c_str()<<":"<<attr->MakeToString().c_str()<<std::endl;
        });

        //传统方式
//         std::cout<<"name:"<<name->MakeToString().c_str()<<std::endl;
//         std::cout<<" sex:"<< sex->MakeToString().c_str()<<std::endl;
//         std::cout<<" age:"<< age->MakeToString().c_str()<<std::endl;
    }

    return(0);
}
