#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<hgl/type/SortedSets.h>
#include<hgl/type/Map.h>
#include<hgl/type/String.h>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void out_id(Map<int,int> &int_map)
{
    const int count=int_map.GetCount();

    cout<<"count:"<<count<<" ";

//传统用法
//    Pair<int,int> **p=int_map.GetDataList();

//     for(int i=0;i<count;i++)
//     {
//         cout<<"["<</*i<<":"<<*/(*p)->left<<","<<(*p)->right<<"]";
//         ++p;
//     }

    //lambda用法
    int_map.Enum([](const int &key,int &value)
    {
        cout<<"["<<key<<","<<value<<"]";
    });

    cout<<endl;
}

void IntMapTest()
{
    constexpr const int MAX_COUNT=10;

    int index;
    int value;
    SortedSets<int> int_sets;
    Map<int,int> int_map;

    srand(time(nullptr));

    for(int i=0;i<MAX_COUNT;i++)
    {
        while(true)
        {
            index=rand()%MAX_COUNT;
            if(!int_sets.Contains(index))break;
        }

        int_sets.Add(index);

        value=rand()%255;

        cout<<"add index="<<index<<" value="<<value<<endl;
        int_map.Add(index,value);

        out_id(int_map);
    }

    cout<<"-----------------------------------------"<<endl;
    cout<<"Map<int,int> Change Test"<<endl;

    for(int i=0;i<MAX_COUNT;i++)
    {
        value=rand()%255;

        cout<<"update index="<<i<<" value="<<value<<endl;
        int_map.Change(i,value);

        out_id(int_map);
    }
}

void StringMapTest()
{
    Map<UTF8String,UserInfo> ui_map;

    for(auto &ui:user_info_array)
        ui_map.Add(ui.name,ui);

    ui_map.Enum([](const UTF8String &key,UserInfo &ui)
    {
        cout<<"["<<key<<","<<(ui.sex?"male":"female")<<","<<ui.age<<"]"<<endl;
    });

    cout<<endl;
}

void StringObjectMapTest()
{
    ObjectMap<UTF8String,UserInfoClass> ui_map;

    for(auto &ui:user_info_array)
    {
        UserInfoClass *uic=new UserInfoClass;

        uic->Set(ui);

        ui_map.Add(ui.name,uic);
    }

    ui_map.Enum([](const UTF8String &key,UserInfoClass *&ui)
    {
        cout<<"["<<key<<","<<(ui->GetSex()?"male":"female")<<","<<ui->GetAge()<<"]"<<endl;
    });
}

int main(int,char **)
{
    cout<<"-----------------------------------------"<<endl;
    cout<<"Map<int,int> Test"<<endl;
    IntMapTest();

    cout<<"-----------------------------------------"<<endl;
    cout<<"Map<String,UserInfo> Test"<<endl;
    StringMapTest();

    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectMap<String,UserInfoClass> Test"<<endl;
    StringObjectMapTest();

    return(0);
}
