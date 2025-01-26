#include<iostream>
#include<hgl/type/object/Object.h>

using namespace hgl;

class TestObject:public Object
{
public:

    using Object::Object;
    virtual ~TestObject()=default;
};

int main(int argc,char **)
{
    NewObject(TestObject,to);

    const auto &obi=to->GetObjectBaseInfo();

    std::cout<<"file: "<<obi.scl.file<<std::endl
             <<"line: "<<obi.scl.line<<std::endl
             <<"func: "<<obi.scl.func<<std::endl;

    return 0;
}
