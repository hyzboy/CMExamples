#include<hgl/filesystem/Filename.h>
#include<iostream>

using namespace std;
using namespace hgl;
using namespace hgl::filesystem;

void main()
{
    constexpr os_char incorrect_filename[]=OS_TEXT("c:/win95\\\\ms-dos");

    os_out<<OS_TEXT("fix filename test that they have a few problems.\n\n");

    os_out<<OS_TEXT("origin filename: ")<<incorrect_filename<<endl;
    os_out<<OS_TEXT(" fixed filename: ")<<FixFilename(incorrect_filename).c_str()<<endl;
}