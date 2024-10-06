#include<iostream>
#include<hgl/math/MathConst.h>

using namespace std;
using namespace hgl;

int main(int,char **)
{
    std::cout<<"float min:"<<HGL_FLOAT_MIN<<std::endl;
    std::cout<<"float max:"<<HGL_FLOAT_MAX<<std::endl;
    std::cout<<"float epsilon: "<<HGL_FLOAT_EPSILON<<std::endl;

    std::cout<<"double min:"<<HGL_DOUBLE_MIN<<std::endl;
    std::cout<<"double max:"<<HGL_DOUBLE_MAX<<std::endl;
    std::cout<<"double epsilon: "<<HGL_DOUBLE_EPSILON<<std::endl;

    return 0;
}