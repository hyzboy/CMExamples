#include<iostream>
#include<hgl/math/MathConst.h>

using namespace std;
using namespace hgl;

int main(int,char **)
{
    std::cout<<"float: "<<HGL_FLOAT_EPSILON<<std::endl;
    
    std::cout<<"double: "<<HGL_DOUBLE_EPSILON<<std::endl;

    return 0;
}