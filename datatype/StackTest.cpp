#include<hgl/type/Stack.H>
#include<iostream>

using namespace hgl;
using namespace std;

void TestStack()
{
    Stack<int> tab;
    int i;

    for(i=0;i<10;i++)
    {
        cout<<"push "<<i<<endl;

        tab.Push(i);
    }

    cout<<"Stack Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        int val;

        tab.Pop(val);
        
        cout<<"pop "<<val<<endl;
    }

    cout<<"Stack Count: "<<tab.GetCount()<<endl;
}

class StackTestObject
{
    int val;

public:

    StackTestObject(int v)
    {
        val=v;

        cout<<"StackTestObject "<<val<<endl;
    }

    ~StackTestObject()
    {
        cout<<"~StackTestObject "<<val<<endl;
    }
};

void TestObjectStack()
{
    ObjectStack<StackTestObject> tab;

    int i;

    for(i=0;i<10;i++)
    {
        tab.Push(new StackTestObject(i));
    }

    cout<<"Stack Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        StackTestObject *obj=tab.Pop();

        if(obj)
            delete obj;
    }

    cout<<"Stack Count: "<<tab.GetCount()<<endl;
}

int os_main(int,os_char **)
{
    TestStack();

    cout<<"------------------------"<<endl;

    TestObjectStack();

    return(0);
}
