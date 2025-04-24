#include<hgl/type/IndexedList.h>
#include<random>
#include<iostream>
#include<iomanip>

using namespace hgl;

template<typename T>
void out_list(const char *type,const DataArray<T> &da)
{
    const T *p=da.GetData();

    std::cout<<type<<"[";

    for(int32 i=0;i<da.GetCount();i++)
    {
        if(i>0)
            std::cout<<",";
        
        std::cout<<*p;

        ++p;
    }

    std::cout<<"]";
}

void out_list(const char *info,const IndexedList<int> &il)
{
    const int32 count=il.GetCount();

    std::cout<<info;

    out_list(" Index",il.GetRawIndex());
    out_list(" RawData",il.GetRawData());

    {
        std::cout<<" IL[";

        for(auto it:il)
        {
            std::cout<<" "<<it;
        }

        std::cout<<"]";
    }

    std::cout<<std::endl;
}

int main(int,char **)
{
    std::random_device device;
    std::mt19937 generator(device());

    IndexedList<int> il;

    for(int i=0;i<10;i++)
        il.Add(generator()%100);

    out_list("origin",il);

    for(int i=0;i<5;i++)
    {
        int pos=generator()%il.GetCount();

        std::cout<<"Delete "<<pos<<std::endl;
        il.Delete(pos);
        out_list("delete",il);
    }
}
