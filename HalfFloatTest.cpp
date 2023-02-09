#include<hgl/math/HalfFloat.h>
#include<iostream>
#include<cstdlib>
#include<cmath>

using namespace hgl;
using namespace std;

static float origin_float[4];

void InitFloat4()
{
    //随机产生4个f32浮点数
    {
        srand(time(nullptr));

        uint8 *p=(uint8 *)origin_float;
    
        for(uint i=0;i<16;i++)
            p[i]=rand()%0xFF;
    }

    for(uint i=0;i<4;i++)
    {
        origin_float[i]=abs(origin_float[i]);
    }
}

void OutputFloat4(const char *hint,const float *f)
{
    cout<<hint<<" float4: "<<f[0]<<","<<f[1]<<","<<f[2]<<","<<f[3]<<endl;
}

void OutputHalfFloat(const char *hint,const half_float *hf)
{
    cout<<hint<<" half_float: "<<hf[0]<<","<<hf[1]<<","<<hf[2]<<","<<hf[3]<<endl;
}

void half_float_test()
{
    OutputFloat4("origin",origin_float);

    half_float hf_fast[4];
    half_float hf_std[4];

    float_to_half(hf_fast,origin_float,4);
    Float32toFloat16(hf_std,origin_float,4);

    for(uint i=0;i<4;i++)
        hf_std[i]&=0x7FFF;      //去掉符号位

    OutputHalfFloat("fast",hf_fast);
    OutputHalfFloat("std ",hf_std);
}

void main(int,char **)
{
    InitFloat4();

    half_float_test();
}
