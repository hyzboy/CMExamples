#include<hgl/type/RectScope.h>
#include<hgl/graph/Triangle.h>
#include<hgl/graph/Ray.h>
#include<hgl/2d/TGA.h>
#include<hgl/2d/Bitmap.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/DrawGeometry.h>
#include<hgl/time/Time.h>
#include<iostream>
#include<random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis_01(0, 1);
std::uniform_int_distribution<> dis_int(0, 1023);

using namespace hgl;
using namespace std;

void Pick2DRectangle()
{
    RectScope2f rs;
    
    rs.SetPosition(0,0);
    rs.SetSize(1,1);

    rs.PointIn(hgl::Vector2f(0.5,0.5));
}

void Pick2DTriangle()
{
    graph::Triangle2i tri;

    Vector2i v[3];
    Vector3i edge_length;
    Vector2i t;
    
    for(int i=0;i<3;i++)
    {
        v[i].x=dis_int(gen);
        v[i].y=dis_int(gen);

        tri.SetVertex(i,v[i]);

        std::cout<<"triangle "<<i<<"："<<v[i].x<<","<<v[i].y<<std::endl;
    }

    edge_length.x=tri.GetEdgeLength(0);
    edge_length.y=tri.GetEdgeLength(1);
    edge_length.z=tri.GetEdgeLength(2);

    std::cout<<"triangle area(method 1): "<<TriangleArea(edge_length.x,edge_length.y,edge_length.z)<<std::endl;
    std::cout<<"triangle area(method 2): "<<tri.Area()<<std::endl;

    bitmap::BitmapRGB8 bmp;
    Vector3u8 clear_color(0,0,0);

    bmp.Create(1024,1024);
    bmp.ClearColor(clear_color);

    bitmap::DrawGeometryRGB8 draw(&bmp);

    draw.SetDrawColor(Vector3u8(255,255,255));

    draw.DrawLine(v[0],v[1]);
    draw.DrawLine(v[1],v[2]);
    draw.DrawLine(v[2],v[0]);

    double st=GetPreciseTime();

    for(int i=0;i<10000;i++)
    {
        t.x=dis_int(gen);
        t.y=dis_int(gen);

        if(tri.PointIn(t))
            draw.SetDrawColor(Vector3u8(0,255,0));
        else
            draw.SetDrawColor(Vector3u8(0,0,255));

        draw.PutPixel(t);
    }

    double et=GetPreciseTime();

    std::cout<<"test 10000 points in a triangle cost "<<(et-st)<<" seconds."<<std::endl;

    bitmap::SaveBitmapToTGA(OS_TEXT("Triangle.tga"),&bmp);
}

int main(int,char **)
{
    Pick2DTriangle();

    return 0;
}