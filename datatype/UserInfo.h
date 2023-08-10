#pragma once

#include<string.h>
#include<iostream>

using namespace std;

struct UserInfo
{
    char name[32];
    bool sex;
    int age;
};

static UserInfo user_info_array[]=
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

class UserInfoClass
{
    UserInfo info;

public:

    UserInfoClass()=default;

    void Set(const UserInfo &ui)
    {
        memcpy(&info,&ui,sizeof(UserInfo));

        cout<<"UserInfoClass::Set("<<info.name<<")"<<endl;
    }

    ~UserInfoClass()
    {
        cout<<"~UserInfoClass("<<info.name<<")"<<endl;
    }

    const char *GetName()const{return info.name;}
    const bool GetSex()const{return info.sex;}
    const int GetAge()const{return info.age;}
};