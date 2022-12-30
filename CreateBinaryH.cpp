#include<hgl/math/Binary.h>
#include<iostream>

using namespace hgl;
using namespace std;

void main()
{
    char str[9];

    str[8]=0;

    for(unsigned int i=0;i<256;i++)
    {
        Integer2Binary(str,i);

        cout<<"constexpr uint8 HGL_B"<<str<<"="<<i<<";"<<endl;
    }
}