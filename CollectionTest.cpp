#include<hgl/type/Collection.h>
#include<iostream>

using namespace hgl;
using namespace std;

template<typename T> class NewList:public Collection<T>
{

public:

    NewList(MemoryBlock *amb=new MemoryBlock(new MemoryAllocator)):Collection(amb)
    {
    }

    virtual ~NewList()=default;


};//

void main(int,char **)
{
    {
        MemoryAllocator *ma=new MemoryAllocator;

        ma->Alloc(1024);

        MemoryBlock mb(ma);
    }

    {
        cout<<"Collection"<<endl<<endl;

        NewList<uint32> ulist;

        ulist.Add(1);
    }
}
