#include<iostream>
#include<hgl/platform/CpuInfo.h>

void main()
{
    hgl::CpuInfo ci;

    hgl::GetCpuInfo(&ci);

    std::cout<<"CPU: "<<ci.cpu_count<<std::endl;    
    std::cout<<"Core: "<<ci.core_count<<std::endl;
    std::cout<<"Logical Core: "<<ci.logical_core_count<<std::endl;

    std::cout<<std::endl;

    return;
}