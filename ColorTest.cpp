#include<hgl/color/ColorFormat.h>
#include<iostream>

using namespace hgl;
using namespace std;

void half_float_test()
{
    const float         f_12_34 =12.34f;
    const half_float    hf_12_23=float2half(12.34f);
    const float         f_12_23_hf=half2float(hf_12_23);

    cout<<"f_12_34="<<f_12_34<<endl;
    cout<<"hf_12_23="<<hf_12_23<<endl;
    cout<<"f_12_23_hf="<<f_12_23_hf<<endl;
}

void main(int,char **)
{
    half_float_test();
}
