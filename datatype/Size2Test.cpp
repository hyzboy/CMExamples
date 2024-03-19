﻿#include<hgl/type/Size2.h>
#include<iostream>

using namespace hgl;
using namespace std;

Size2i si_screen,si_canvas,
        si_finish,si_gap;

void output(const char *str,float scale)
{
    cout<<endl<<str<<" scale "<<scale<<endl;

    si_finish=si_canvas*scale;

    cout<<"    finish size: "<<si_finish.width<<","<<si_finish.height<<endl;

    //si_gap=(si_finish-si_screen)/2;
    si_gap=(si_canvas*scale-si_screen)/2;       //写这行而不是用上一行，是为了验证操作符重载的正确性

    cout<<"    gap size: "<<si_gap.width<<","<<si_gap.height<<endl;
}

void main()
{
    si_screen.Set(1920,1200);
    si_canvas.Set(1280,720);

    cout<<"screen size: "<<si_screen.width<<","<<si_screen.height<<endl;
    cout<<"canvas size: "<<si_canvas.width<<","<<si_canvas.height<<endl;

    output("min",si_screen.alcScale(si_canvas,false));
    output("max",si_screen.alcScale(si_canvas,true));
}
