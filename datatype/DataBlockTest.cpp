#include<iostream>
#include"DataChain.h"

using namespace std;
using namespace hgl;

void TestSeriesPool()
{
    SeriesPool<int> sp(10);

    cout<<"sp.GetMaxCount()="<<sp.GetMaxCount()<<endl;

    sp.Init(20);

}


int os_main(int,os_char **)
{
}