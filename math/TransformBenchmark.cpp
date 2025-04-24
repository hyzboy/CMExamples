#include<hgl/Time.h>
#include<hgl/math/Transform.h>
#include<iostream>
#include <random>

using namespace hgl;

constexpr const size_t TEST_COUNT=1000000;

int main(int,char **)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis_01(0, 1);
    std::uniform_real_distribution<> dis_360(0, 359);

    Transform *t=new Transform[TEST_COUNT],*tp;
    Matrix4f *m=new Matrix4f[TEST_COUNT],*mp;

    tp=t;
    mp=m;

    double st,et;

    Vector3f r_vec;

    for(int i=0;i<TEST_COUNT;i++)
    {
        r_vec.x=dis_01(gen);
        r_vec.y=dis_01(gen);
        r_vec.z=dis_01(gen);

        tp->SetRotation(r_vec,dis_360(gen));

        r_vec.x=dis_360(gen);
        r_vec.y=dis_360(gen);
        r_vec.z=dis_360(gen);

        tp->SetScale(r_vec);

        r_vec.x=dis_360(gen);
        r_vec.y=dis_360(gen);
        r_vec.z=dis_360(gen);

        tp->SetTranslation(r_vec);

        ++tp;
    }

    tp=t;

    st=GetPreciseTime();
    for(int i=0;i<TEST_COUNT;i++)
    {
        *m=tp->GetMatrix();

        ++tp;
        ++m;
    }
    et=GetPreciseTime();

    std::cout<<"Transform::GetMatrix() "<<TEST_COUNT<<" time: "<<et-st<<"sec."<<std::endl;
    return 0;
}