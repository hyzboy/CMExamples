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

void OutputFloat4(const float *f)
{
    cout<<" f32: "<<f[0]<<","<<f[1]<<","<<f[2]<<","<<f[3]<<endl;
}

void OutputHalfFloat(const half_float *hf)
{
    cout<<" f16: "<<hf[0]<<","<<hf[1]<<","<<hf[2]<<","<<hf[3]<<endl;
}

void half_float_test()
{
    OutputFloat4(origin_float);

    half_float hf_std[4];

    Float32toFloat16(hf_std,origin_float,4);

    OutputHalfFloat(hf_std);
}

void split_float_test()
{
    bool sign_bit;
    uint exponent;
    uint mantissa;

    float result;

    for(uint i=0;i<4;i++)
    {
        SplitFloat32(sign_bit,exponent,mantissa,origin_float[i]);

        result=MergeFloat32(sign_bit,exponent,mantissa);

        cout<<"origin: "<<origin_float[i]<<" result: "<<result<<" sign_bit:"<<sign_bit<<" exponent:"<<exponent<<" mantissa:"<<mantissa<<endl;
    }
}

void main(int,char **)
{
    InitFloat4();

    half_float_test();

    split_float_test();
}
