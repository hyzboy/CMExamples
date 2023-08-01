#include<hgl/type/LifetimeCallback.h>

#include<iostream>
#include<stdlib.h>

using namespace hgl;

template<typename T,typename TCB> class TestArray
{
    TCB *life_cb;

    T *items;
    int count;
    int alloc_count;

private:

    void Realloc(int new_count)
    {
        if(new_count<alloc_count)
            return;

        alloc_count=new_count;

        if(items)
            items=(T *)realloc(items,sizeof(T)*alloc_count);
        else
            items=(T *)malloc(sizeof(T)*alloc_count);
    }

public:

    T *begin(){return items;}
    T *end(){return items+count;}

public:

    TestArray(RawLifetimeCallback<T> *cb)
    {
        life_cb=cb;

        items=nullptr;
        count=0;
    }

    ~TestArray()
    {
        if(items)free(items);
    }

    T *Create()
    {
        int off=count;

        ++count;

        Realloc(count);

        T *p=items+off;

        life_cb->Create(p);

        return p;
    }

    void Add(T *data,const int num)
    {
        int off=count;

        count+=num;

        Realloc(count);

        life_cb->Copy(items+off,data,num);
    }

    void Add(T data)
    {
        Add(&data,1);
    }
};

template<typename T> class TestRawArray:public TestArray<T,RawLifetimeCallback<T>>
{
    RawLifetimeCallback<T> life_cb;

public:

    TestRawArray():TestArray<T,RawLifetimeCallback<T>>(&life_cb){}
    ~TestRawArray()=default;
};

template<typename T> class TestObject
{
    int value;

public:

    TestObject(){value=0;}
    ~TestObject()
    {
        std::cout<<"~TestObject("<<value<<")"<<std::endl;
    }
};

template<typename T> class TestObjectArray:public Test<T,ObjectLifetimeCallback<T>>
{
    DefaultObjectLifetimeCallback life_cb;;

public:

    TestObjectArray():TestArray<T,ObjectLifetimeCallback<T>>(&life_cb){}
    ~TestObjectArray()=default;
};

void main()
{
    //原生类型测试
    {
        TestRawArray<int> ta_int;

        ta_int.Add(1);
        ta_int.Add(2);
        ta_int.Add(3);

        *(ta_int.Create())=4;

        int two[2]={9,0};

        ta_int.Add(two,2);

        std::cout<<"TestArray<int>: ";

        for(const int val:ta_int)
            std::cout<<val<<" ";

        std::cout<<std::endl;
    }

    //原生结构测试
    {
        struct DataStruct
        {
            int a,b;
        };

        TestRawArray<DataStruct> ta_ds;

        DataStruct ds{1,2};

        ta_ds.Add(ds);

        DataStruct *p=ta_ds.Create();

        *p={3,4};

        DataStruct ds2[2]{{5,6},{7,8}};

        ta_ds.Add(ds2,2);

        std::cout<<"TestArray<struct<int,int,int>>: ";

        for(const DataStruct &ds:ta_ds)
            std::cout<<"{"<<ds.a<<","<<ds.b<<"} ";

        std::cout<<std::endl;
    }

    //对象测试（构造函数无参数)
    {
        TestObjectArray<TestObject<int>> ta_obj;

        ta_obj.Add();
        ta_obj.Add();
        ta_obj.Add();

        std::cout<<"TestArray<TestObject<int>>: ";

        for(const TestObject<int> &obj:ta_obj)
            std::cout<<"{"<<obj.value<<"} ";

        std::cout<<std::endl;
    }
}
