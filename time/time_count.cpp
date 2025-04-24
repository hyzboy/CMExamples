#include<hgl/Time.h>
#include<hgl/type/DateTime.h>
#include<hgl/type/String.h>
#include<iostream>

using namespace hgl;
using namespace std;

constexpr const size_t TIME_OUT_FRAC=6;     //小数输出位数

int main(int,char **)
{
    const double cur_time=GetPreciseTime();

    Date d;
    Time t;

    ToDateTime(d,t,cur_time);

    U8String str=U8String::floatOf(cur_time,TIME_OUT_FRAC);

    cout<<"cur_time="<<str.c_str()<<endl;
    cout<<"\tyear="<<d.GetYear()<<" month="<<d.GetMonth()<<" day="<<d.GetDay()<<endl;
    cout<<"\thour="<<t.GetHour()<<" minute="<<t.GetMinute()<<" second="<<t.GetSecond()<<" micro_second="<<t.GetMicroSecond()<<endl;
    cout<<endl;

    const double new_time=FromDateTime(d,t);

    str=U8String::floatOf(new_time,TIME_OUT_FRAC);
    cout<<"new_time="<<str.c_str()<<endl;

    cout<<"TimeZone:"<<GetTimeZone()<<endl;
    return(0);
}
