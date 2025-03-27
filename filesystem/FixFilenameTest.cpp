#include<hgl/filesystem/Filename.h>
#include<iostream>

using namespace std;
using namespace hgl;
using namespace hgl::filesystem;

int os_main(int,os_char **)
{
    constexpr os_char incorrect_filename[]=OS_TEXT("c:/win95\\\\ms-dos");

    os_out<<OS_TEXT("fix filename test that they have a few problems.\n\n");

    os_out<<OS_TEXT("origin filename: ")<<incorrect_filename<<endl;
    os_out<<OS_TEXT(" fixed filename: ")<<FixFilename(incorrect_filename).c_str()<<endl;

    cout<<"Combine(1,2,3): "<<Combine({"1","2","3"}).c_str()<<endl;

    os_out<<OS_TEXT("Combine(A,B,C): ")<<Combine({OS_TEXT("A"),OS_TEXT("B"),OS_TEXT("C")}).c_str()<<endl;

    os_out<<OS_TEXT("Combine(a,b,c): ")<<Combine({OSString(OS_TEXT("A")),OSString(OS_TEXT("B")),OSString(OS_TEXT("C"))}).c_str()<<endl;

    return 0;
}