#include<hgl/type/Queue.h>
#include<iostream>

using namespace hgl;
using namespace std;

void TestQueueOrdered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Ordered Test"<<endl<<endl;

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

void TestQueueUnordered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Unordered Test"<<endl<<endl;

    Queue<int> tab;
    int i;
    int val;

    for(i=0;i<20;i++)
    {
        if(rand()&1)
        {
            if(tab.Pop(val))        
                cout<<"pop "<<val<<endl;
            else
                cout<<"pop error"<<endl;
        }
        else
        {
            cout<<"push "<<i<<endl;

            tab.Push(i);
        }
    }
}

struct UserInfo
{
    char name[32];
    bool sex;
    int age;
};

static UserInfo ui_array[]=
{
    //注： 以下人物信息由Github Copilot自动创建
    //ps: The following list of character information is automatically created by Github Copilot

    {"Adloph",true,18},
    {"Bella",false,19},
    {"Cindy",false,20},
    {"David",true,21},
    {"Elsa",false,22},
    {"Frank",true,23},
    {"Gina",false,24},
    {"Helen",false,25},
    {"Ivan",true,26},
    {"Jack",true,27},
    {"Kitty",false,28},
    {"Lily",false,29},
    {"Mike",true,30},
    {"Nancy",false,31},
    {"Owen",true,32},
    {"Peter",true,33},
    {"Queen",false,34},
    {"Robert",true,35},
    {"Sunny",false,36},
    {"Tom",true,37},
    {"Uma",false,38},
    {"Vivian",false,39},
    {"Wendy",false,40},
    {"Xavier",true,41},
    {"Yoyo",false,42},
    {"Zack",true,43}
};

void TestQueueStruct()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Struct Test"<<endl<<endl;

    Queue<UserInfo> ui_queue;

    for(uint i=0;i<sizeof(ui_array)/sizeof(UserInfo);i++)
        ui_queue.Push(ui_array[i]);

    cout<<"Queue Count: "<<ui_queue.GetCount()<<endl;

    for(uint i=0;i<sizeof(ui_array)/sizeof(UserInfo);i++)
    {
        UserInfo ui;

        ui_queue.Pop(ui);

        cout<<i<<": "<<ui.name<<(ui.sex?" male ":" female ")<<ui.age<<" age."<<endl;
    }
}

class QueueTestObject
{
    int val;

public:

    QueueTestObject(){val=-1;}

    void Set(int v)
    {
        val=v;

        cout<<"QueueTestObject::Set "<<val<<endl;
    }

    ~QueueTestObject()
    {
        cout<<"~QueueTestObject "<<val<<endl;
    }
};

void TestObjectQueue()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Object Test"<<endl<<endl;

    ObjectQueue<QueueTestObject> tab;

    int i;

    for(i=0;i<10;i++)
    {
        QueueTestObject *obj=new QueueTestObject;

        obj->Set(i);

        tab.Push(obj);
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<5;i++)        //只取出5个,剩几个给自动清理处理
    {
        QueueTestObject *obj=tab.Pop();

        if(obj)
            delete obj;
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;
}

int os_main(int,os_char **)
{
    srand(time(nullptr));

    //原生单个数据测试
    TestQueueOrdered();
    TestQueueUnordered();

    //原生结构体测试
    TestQueueStruct();

    //对象测试
    TestObjectQueue();

    return(0);
}
