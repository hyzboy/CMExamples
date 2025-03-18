#include<iostream>
#include<string>
#include<hgl/SourceCodeLocation.h>

using namespace std;
using namespace hgl;

void out(const string &name,const SourceCodeLocation &scl)
{
    cout<<name<<": "<<scl.file<<" ("<<scl.line<<"): \""<<scl.func<<'"'<<endl;
}

class TestObject
{
    string name;
    SourceCodeLocation scl;

public:

    TestObject(const SourceCodeLocation &_scl,const string &n)
    {
        name=n;
        scl=_scl;
    }

    void out()
    {
        ::out(name,scl);
    }
};

#define NewObject(class_name,...)  new class_name(HGL_SCL_HERE __VA_OPT__(,) __VA_ARGS__)

int main(int,char **)
{
    SourceCodeLocation scl=HGL_SCL_HERE;
    
    out("C++",scl);

    TestObject to(HGL_SCL_HERE,"TO1");

    to.out();

    auto to2=NewObject(TestObject,"NEW");

    to2->out();
    
    return 0;
}