#include<hgl/type/Collection.h>
//#include<hgl/type/List.h>
#include<iostream>

using namespace hgl;
using namespace std;

using               TEST_TYPE   =char;
using               MyEnumerator=ElementEnumerator<TEST_TYPE>;
using               MyCheck     =CheckElementEqual<TEST_TYPE>;

constexpr   size_t  UNIT_BYTES  =sizeof(TEST_TYPE);

void out(const Collection &c)
{
    MyEnumerator me(&c);

    const TEST_TYPE *str=me.begin();
    const size_t count=me.size();

    for(size_t i=0;i<count;i++)
    {
        std::cout<<*str<<' ';
        ++str;
    }

//    for(const TEST_TYPE &value:me)
//        std::cout<<value<<" ";

    std::cout<<std::endl;
}

void main(int,char **)
{
     {
        Collection cu(UNIT_BYTES);

        for(TEST_TYPE i=0;i<10;i++)
        {
            cu.AddValue<TEST_TYPE>('A'+i);
            out(cu);
        }

        cu.Insert(0,0);out(cu);

        std::cout<<"indexOf(E) is "<<cu.indexOf('E')<<std::endl;
        
        std::cout<<"remove second data."<<std::endl;
        cu.RemoveAt(1);out(cu);

        std::cout<<"remove all 'A'"<<std::endl;
        cu.Remove('A');out(cu);
         
        std::cout<<"remove 3 data from third"<<std::endl;
        cu.RemoveAt(2,3);out(cu);

        {
            Collection del_cu(UNIT_BYTES);
            CheckElementEqual<TEST_TYPE> cee;

            del_cu.AddValue<TEST_TYPE>('I');
            del_cu.AddValue<TEST_TYPE>('F');

            std::cout<<"remove all 'I' and 'F'"<<std::endl;
            cu.RemoveCollection(del_cu,&cee);out(cu);
        }

        std::cout<<"exchanged two elements, they at 0 and 2"<<std::endl;
        cu.Exchange(0,2);out(cu);
    }

    {
        //List<uint32> ul;

        //ul.Add(1024);
    }
}
