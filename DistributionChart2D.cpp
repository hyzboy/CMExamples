#include<hgl/type/StringList.h>
#include<hgl/type/LoadStringList.h>
#include<hgl/math/Vector.h>
#include<hgl/util/imgfmt/tga.h>
#include<hgl/io/FileOutputStream.h>
#include<iostream>

using namespace hgl;

bool ParseLine(Vector2i *result,const UTF8String &str)
{
    if(!result)return(false);

    if(str.Length()<=0)return(false);

    const char *sp=str.c_str();
    const char *cp;

    if(*sp!='X')return(false);

    sp+=2;
    cp=hgl::strchr(sp,'Y');

    if(!cp)return(false);

    if(!hgl::stoi(sp,result->x))
        return(false);

    sp=cp+2;

    cp=hgl::strchr(sp,'Z');

    if(!cp)return(false);

    if(!hgl::stoi(sp,result->y))
        return(false);

    return(true);
}

struct PositionStat
{
    uint count;

    Vector2i minp,maxp;
    Vector2i gap;

    Vector2i *data;

public:

    PositionStat(const uint c)
    {
        count=c;

        hgl_zero(minp);
        hgl_zero(maxp);
        hgl_zero(gap);

        data=new Vector2i[count];

        hgl_zero(data,count);
    }

    ~PositionStat()
    {
        delete[] data;
    }
};

PositionStat *ToVector2i(const UTF8StringList &sl)
{
    const uint count=sl.GetCount();

    PositionStat *ps=new PositionStat(count);

    Vector2i *p=ps->data;

    UTF8String str;
    uint result=0;

    for(int i=0;i<count;i++)
    {
        str=sl[i];

        if(str.Length()<=0)
            continue;

        if(!ParseLine(p,str))
            continue;

        (*p)/=100;      //Unreal单位为cm,把单位缩到米

        if(p->x>4096
         ||p->y>4096)
            continue;

        (*p)/=4;

        //std::cout<<"X="<<p->x<<",Y="<<p->y<<std::endl;

//        if(p->x<ps->minp.x)ps->minp.x=p->x;
        if(p->x>ps->maxp.x)ps->maxp.x=p->x;
//        if(p->y<ps->minp.y)ps->minp.y=p->y;
        if(p->y>ps->maxp.y)ps->maxp.y=p->y;
        
        ++result;
        ++p;
    }

    //std::cout<<"minp: "<<ps->minp.x<<","<<ps->minp.y<<std::endl;
    std::cout<<"maxp: "<<ps->maxp.x<<","<<ps->maxp.y<<std::endl;

    ps->count=result;

    return ps;
}

struct Chart
{
    uint width,height;

    uint min_count;
    uint max_count;
    uint gap;

    uint32 *count_data;
    uint32 *circle_data;
    uint8 *chart_data;

public:

    Chart(const uint w,const uint h)
    {
        width=w;
        height=h;

        count_data=new uint32[width*height];
        circle_data=new uint32[width*height];
        chart_data=new uint8[width*height*4];

        hgl_zero(count_data,width*height);
        hgl_zero(circle_data,width*height);
        hgl_zero(chart_data,width*height*4);

        min_count=0;
        max_count=0;
        gap=0;
    }

    ~Chart()
    {
        delete[] count_data;
        delete[] circle_data;
        delete[] chart_data;
    }

    void Circle(uint x,uint y,uint radius)
    {
        uint r2=radius*radius;
        uint length;

        for(int col=x-radius;col<=x+radius;col++)
        {
            if(col<0||col>=width)continue;

            for(int row=y-radius;row<=y+radius;row++)
            {
                if(row<0||row>=height)continue;

                length=(col-x)*(col-x)+(row-y)*(row-y);
                
                if(length<=r2)
                    ++circle_data[col+row*width];
            }
        }
    }
};

Chart *ToChart32(const PositionStat *ps)
{
    uint width=ps->maxp.x-ps->minp.x+1;
    uint height=ps->maxp.y-ps->minp.y+1;

    std::cout<<"width: "<<width<<",height: "<<height<<std::endl;    

    Chart *chart=new Chart(width,height);

    //统计每个格子数据数量
    {
        uint x,y;
        uint32 *cp32=chart->count_data;

        const Vector2i *p=ps->data;

        for(uint i=0;i<ps->count;i++)
        {
            x=p->x-ps->minp.x;
            y=p->y-ps->minp.y;

            cp32=chart->count_data+(x+y*width);

            if(*cp32<0xFFFFFFFF)
                ++(*cp32);

            ++p;
        }
    }

    //画圆
    {
        uint32 *cp32=chart->count_data;

        for(uint y=0;y<height;y++)
        {
            for(uint x=0;x<width;x++)
            {
                if(*cp32>0)
                    chart->Circle(x,y,(*cp32)*10);

                ++cp32;
            }
        }
    }

    //统计最小值、最大值
    {
        uint32 *cp32=chart->circle_data;

        for(uint i=0;i<width*height;i++)
        {
            if(*cp32<chart->min_count)chart->min_count=*cp32;
            if(*cp32>chart->max_count)chart->max_count=*cp32;

            ++cp32;
        }

        chart->gap=chart->max_count-chart->min_count;

        std::cout<<"min_count: "<<chart->min_count<<",max_count: "<<chart->max_count<<",gap: "<<chart->gap<<std::endl;
    }

    //生成权重图
    {
        uint32 *cp32=chart->circle_data;
        uint8 *cp8=chart->chart_data;

        float result;
        float r,g,b;

        //1.0   1 0 0
        //0.8   1 1 0
        //0.6   0 1 0
        //0.4   0 1 1
        //0.2   0 0 1
        //0.0   0 0 0

        for(uint i=0;i<width*height;i++)
        {
            result=float((*cp32)-chart->min_count)/float(chart->gap);

            if(result>=0.8f)       //0.8 - 1.0
            {
                r=1.0f;
                g=1.0f-(result-0.8f)*5.0f;
                b=0.0f;
            }
            else
            {
                if(result>=0.6f)    //0.6 - 0.8
                {
                    r=(result-0.6f)*5.0f;
                    g=1.0f;
                    b=0;
                }
                else
                {
                    if(result>=0.4f)   //0.4 - 0.6
                    {
                        r=0.0f;
                        g=1.0f;
                        b=1.0f-(result-0.4f)*5.0f;
                    }
                    else
                    if(result>=0.2f)    //0.2 - 0.4
                    {
                        r=0.0f;
                        g=(result-0.2f)*5.0f;
                        b=1.0f;
                    }
                    else                //0.0 - 0.25
                    {
                        r=0.0f;
                        g=0.0f;
                        b=result*5.0f;
                    }
                }
            }

            cp8[0]=b*255.0f;
            cp8[1]=g*255.0f;
            cp8[2]=r*255.0f;
            cp8[3]=result*255.0f;

            ++cp32;
            cp8+=4;
        }
    }

    return chart;
}

int os_main(int argc,os_char **argv)
{
    std::cout<<"Distribution Chart 2D"<<std::endl<<std::endl;

    if(argc<2)
    {
        std::cout<<"example: DistributionChart2D data.csv"<<std::endl<<std::endl;
        return 0;
    }

    UTF8StringList sl;

    int line_count=LoadStringListFromTextFile(sl,argv[1]);
    
    if(line_count<=1)
    {
        std::cout<<"Load file "<<argv[1]<<" failed!"<<std::endl;
        return(-1);
    }

    std::cout<<"file total line: "<<line_count<<std::endl;

    AutoDelete<PositionStat> ps=ToVector2i(sl);

    AutoDelete<Chart> chart=ToChart32(ps);


    {
        util::TGAHeader tga_header;

        util::FillTGAHeader(&tga_header,chart->width,chart->height,4);

        io::OpenFileOutputStream fos(OS_TEXT("chart.tga"),io::FileOpenMode::CreateTrunc);

        if(!fos)
        {
            std::cerr<<"Create chart.tga failed!"<<std::endl;

            return 1;
        }

        fos->Write(&tga_header,util::TGAHeaderSize);
        fos->Write(chart->chart_data,chart->width*chart->height*4);
        fos->Close();
    }

    return 0;
}