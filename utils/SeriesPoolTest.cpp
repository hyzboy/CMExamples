#include<iostream>
#include<hgl/type/SeriesPool.h>
#include<hgl/u>

using namespace std;
using namespace hgl;

constexpr int POOL_MAX_COUNT=10;

int os_main(int,os_char **)
{
    SeriesU8 sp(POOL_MAX_COUNT);

    cout<<"Series pool MaxCount="<<sp.GetMaxCount()<<endl;


}
