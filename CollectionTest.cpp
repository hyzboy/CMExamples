#include<hgl/type/Collection.h>
//#include<hgl/type/List.h>
#include<iostream>

using namespace hgl;
using namespace std;

using               TEST_TYPE   =uint32;
using               MyEnumerator=ElementEnumerator<TEST_TYPE>;
using               MyOperator  =ElementOperatorRawtype<TEST_TYPE>;
constexpr   size_t  UNIT_BYTES  =sizeof(TEST_TYPE);

void out(const Collection &c)
{
    MyEnumerator me(&c);

    for(const TEST_TYPE &value:me)
        std::cout<<value<<" ";

    std::cout<<std::endl;
}

void main(int,char **)
{
    MyOperator my_operator;
    MemoryAllocator *ma=new MemoryAllocator;

    ma->Alloc(1024*UNIT_BYTES);
    
    MemoryBlock *mb=new MemoryBlock(ma);

    {
        Collection cu(UNIT_BYTES,mb,&my_operator);

        for(uint i=0;i<20;i++)
        {
            cu.Add(i/2);out(cu);
        }

        cu.Insert(0,0);out(cu);

        std::cout<<"indexOf(5) is "<<cu.indexOf(5)<<std::endl;

        cu.RemoveAt(1);out(cu);

        std::cout<<"remove all 0"<<std::endl;
        cu.Remove(0);out(cu);
         
        cu.RemoveAt(2,3);out(cu);
        cu.RemoveAt(3,2);out(cu);
        cu.RemoveAt(3,2);out(cu);

        Collection<uint32> del_cu;

        del_cu.Add(2);
        del_cu.Add(3);

        std::cout<<"remove all 2 and 3"<<std::endl;
        cu.Remove(del_cu);out(cu);

        std::cout<<"exchanged two elements, they at 0 and 2"<<std::endl;
        cu.Exchange(0,2);out(cu);
    }

    {
        //List<uint32> ul;

        //ul.Add(1024);
    }
}
