#include<hgl/type/Queue.h>
#include<iostream>

using namespace hgl;
using namespace std;

void TestQueue()
{
    Queue<int> tab;
    int i;

    for(i=0;i<10;i++)
    {
        cout<<"push "<<i<<endl;

        tab.Push(i);
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        int val;

        tab.Pop(val);
        
        cout<<"pop "<<val<<endl;
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;
}

class QueueTestObject
{
    int val;

public:

    QueueTestObject(int v)
    {
        val=v;

        cout<<"QueueTestObject "<<val<<endl;
    }

    ~QueueTestObject()
    {
        cout<<"~QueueTestObject "<<val<<endl;
    }
};

void TestObjectQueue()
{
    ObjectQueue<QueueTestObject> tab;

    int i;

    for(i=0;i<10;i++)
    {
        tab.Push(new QueueTestObject(i));
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        QueueTestObject *obj=tab.Pop();

        if(obj)
            delete obj;
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;
}

int os_main(int,os_char **)
{
    TestQueue();

    cout<<"------------------------"<<endl;

    TestObjectQueue();

    return(0);
}
