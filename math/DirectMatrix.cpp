#include<hgl/Time.h>
#include<hgl/math/Matrix.h>
#include<iostream>
#include<random>

const hgl::Matrix4f DirectTranslate(const hgl::Vector3f &off)
{
    hgl::Matrix4f result;

    result[0][0]=1; result[1][0]=0; result[2][0]=0; result[3][0]=off.x;
    result[0][1]=0; result[1][1]=1; result[2][1]=0; result[3][1]=off.y;
    result[0][2]=0; result[1][2]=0; result[2][2]=1; result[3][2]=off.z;
    result[0][3]=0; result[1][3]=0; result[2][3]=0; result[3][3]=1;

    hgl::Matrix4f m=hgl::translate(off);

    return(result);
}

int main()
{
    DirectTranslate(hgl::Vector3f(4,5,6));
    return 0;
}
