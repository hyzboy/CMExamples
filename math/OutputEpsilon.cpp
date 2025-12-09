#include<iostream>
#include<hgl/math/FloatPrecision.h>

using namespace std;
using namespace hgl;

int main(int,char **)
{
    std::cout<<"float min:"<<math::float_min<<std::endl;
    std::cout<<"float max:"<<math::float_max<<std::endl;
    std::cout<<"float epsilon: "<<math::float_epsilon<<std::endl;

    std::cout<<"double min:"<<math::double_min<<std::endl;
    std::cout<<"double max:"<<math::double_max<<std::endl;
    std::cout<<"double epsilon: "<<math::double_epsilon<<std::endl;

    return 0;
}