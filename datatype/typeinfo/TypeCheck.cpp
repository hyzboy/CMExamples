#include<iostream>
#include<hgl/type/object/Object.h>

using namespace hgl;
using namespace std;

void out(const char *name,Object *obj)
{
    cout<<name<<" static  "<<obj->GetTypeName()<<": "<<std::hex<<obj->GetTypeHash()<<endl;
}

class TestObject:public Object
{
    HGL_OBJECT_BODY(TestObject)

public:

    HGL_OBJECT_CONSTRUCT(TestObject)
};

class TestObjectA:public TestObject
{
    HGL_OBJECT_BODY(TestObjectA)

public:

    HGL_OBJECT_CONSTRUCT_SC(TestObjectA,TestObject)

};

int main(int,char **)
{
    TestObject *to=NewObject(TestObject);
    TestObjectA *toa=NewObject(TestObjectA);

    out("to",to);
    out("toa",toa);

    return 0;
}
