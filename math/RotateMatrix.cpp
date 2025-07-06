#include<glm/glm.hpp>
#include<glm/gtx/quaternion.hpp>
#include<iostream>

void PrintMatrix(const char *name,const glm::mat4 &m)
{
    std::cout<<"Matrix:"<<name<<"\n";
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
            std::cout<<m[i][j]<<" ";
        std::cout<<"\n";
    }
    std::cout<<"\n";
}

int main(int,char **)
{
    const glm::vec3   rotate_axis(0.0f,1.0f,0.0f); // Y-axis
    const float       rotate_radians=glm::radians(90.0f);

    glm::quat q =glm::angleAxis(rotate_radians,rotate_axis);
    glm::mat4 m1=glm::toMat4(q);

    glm::mat4 m2=glm::rotate(glm::mat4(1.0f),rotate_radians,rotate_axis);

    float cos90=glm::cos(rotate_radians);
    float acos90_1=glm::acos(cos90);
    float acos90_2=glm::acos(5.96046e-08);

    //m1方法结果是5.96046e-08
    //m2方法结果是-4.37114e-08
    //但其实它们结果是一样的，都是近似于0的数值。

    PrintMatrix("From Quaternion",m1);
    PrintMatrix("From Rotate",m2);

    return 0;
}