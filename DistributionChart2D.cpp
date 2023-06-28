#include<hgl/type/StringList.h>
#include<hgl/type/LoadStringList.h>
#include<hgl/type/Gradient.h>
#include<hgl/math/Vector.h>
#include<hgl/util/imgfmt/tga.h>
#include<hgl/io/FileOutputStream.h>
#include<iostream>
#include"BitmapFont.h"

using namespace hgl;

uint CHAR_BITMAP_WIDTH=0;
uint CHAR_BITMAP_HEIGHT=0;
uint CHAR_BITMAP_SCALE=0;
uint CHAR_LINE_HEIGHT=0;

bool InitBitmapFont()
{
    if(!LoadBitmapFont())
        return(false);

    CHAR_BITMAP_WIDTH=GetCharWidth();
    CHAR_BITMAP_HEIGHT=GetCharHeight();

    CHAR_BITMAP_SCALE=1;
    CHAR_LINE_HEIGHT=CHAR_BITMAP_HEIGHT*CHAR_BITMAP_SCALE;

    return(true);
}

constexpr const Vector3u8 white_color={255,255,255};

constexpr const uint STOP_COUNT=5;

constexpr const Vector3u8 stop_color[STOP_COUNT]=
{
    {255,0,0},
    {255,255,0},
    {0,255,0},
    {0,255,255},
    {0,0,255},
};

HGL_GRADIENT_DEFINE(GradientColor3u8,uint,Vector3u8)
{
    result.r=start.r+float(end.r-start.r)*pos;
    result.g=start.g+float(end.g-start.g)*pos;
    result.b=start.b+float(end.b-start.b)*pos;
}

GradientColor3u8 ColorGradient;

void InitGradient(uint max_count)
{
    for(uint i=0;i<STOP_COUNT;i++)
        ColorGradient.Add(max_count*(1.0-float(i)/float(STOP_COUNT-1)),stop_color[i]);
}

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
//        if(p->x>ps->maxp.x)ps->maxp.x=p->x;
//        if(p->y<ps->minp.y)ps->minp.y=p->y;
//        if(p->y>ps->maxp.y)ps->maxp.y=p->y;
        
        ++result;
        ++p;
    }

    //std::cout<<"minp: "<<ps->minp.x<<","<<ps->minp.y<<std::endl;
    //std::cout<<"maxp: "<<ps->maxp.x<<","<<ps->maxp.y<<std::endl;

    ps->maxp.x=1023;
    ps->maxp.y=1023;

    ps->count=result;

    return ps;
}

struct Chart
{
    uint width,height;

    uint max_count;

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

        max_count=0;
    }

    ~Chart()
    {
        delete[] count_data;
        delete[] circle_data;
        delete[] chart_data;
    }

    void DrawPoint(uint x,uint y,uint strong)
    {
        circle_data[x+y*width]+=strong;
    }

    void DrawCircle(uint x,uint y,uint radius)
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
                {
                    ++circle_data[col+row*width];
                }
            }
        }
    }

    void DrawBar(const uint x,const uint y,const uint size,const Vector3u8 &stop_color,const uint8 alpha)
    {
        uint8 *tp=chart_data+(x+y*width)*4;
        uint line_bytes=(width-size)*4;

        for(uint row=0;row<size;row++)
        {
            for(uint col=0;col<size;col++)
            {
                tp[0]=stop_color.b;
                tp[1]=stop_color.g;
                tp[2]=stop_color.r;
                tp[3]=alpha;

                tp+=4;
            }

            tp+=line_bytes;
        }
    }

    void DrawChar(const char ch,const uint x,const uint y,const Vector3u8 &stop_color,const uint8 alpha)
    {
        const uint8 *sp=GetBitmapChar(ch);
        uint8 bit;

        for(uint row=0;row<CHAR_BITMAP_HEIGHT;row++)
        {
            bit=1<<7;

            for(uint col=0;col<CHAR_BITMAP_WIDTH;col++)
            {
                if(*sp&bit)                
                    DrawBar(x+col*CHAR_BITMAP_SCALE,
                            y+row*CHAR_BITMAP_SCALE,
                            CHAR_BITMAP_SCALE,
                            stop_color,
                            alpha);

                bit>>=1;
            }

            ++sp;
        }
    }

    void DrawString(const AnsiString &str,const uint x,const uint y,const Vector3u8 &stop_color,const uint8 alpha)
    {
        const char *sp=str.c_str();
        const uint len=str.Length();
        
        uint pos=x;

        for(uint i=0;i<len;i++)
        {
            if(*sp!=' ')
                DrawChar(*sp,pos,y,stop_color,alpha);

            pos+=CHAR_BITMAP_WIDTH*CHAR_BITMAP_SCALE;
            ++sp;
        }
    }

    void DrawGradient(const uint left,const uint top,const uint w,const uint h)
    {
        uint8 *tp=chart_data+(left+top*width)*4;
        Vector3u8 color;

        uint low,high,gap;
        
        ColorGradient.GetLowest(low);
        ColorGradient.GetHighest(high);

        gap=high-low;

        for(uint i=0;i<h;i++)
        {
            ColorGradient.Get(color,(1.0f-float(i)/float(h))*gap+low);

            for(uint j=0;j<w;j++)
            {
                tp[0]=color.b;
                tp[1]=color.g;
                tp[2]=color.r;
                tp[3]=255;

                tp+=4;
            }

            tp+=(width-w)*4;
        }
    }
};

Chart *ToChart32(const PositionStat *ps)
{
    uint width=ps->maxp.x-ps->minp.x+1;
    uint height=ps->maxp.y-ps->minp.y+1;

    std::cout<<"width: "<<width<<",height: "<<height<<std::endl;    

    Chart *chart=new Chart(width,height);
    uint max_count=0;
    uint step_count[5];

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

            ++(*cp32);

            if(*cp32>max_count)max_count=*cp32;

            ++p;
        }
    }

    //统计占比
    {
        uint32 *cp32=chart->count_data;

        hgl_zero(step_count);

        for(uint y=0;y<height;y++)
        {
            for(uint x=0;x<width;x++)
            {
                if(*cp32>0)
                for(uint i=0;i<STOP_COUNT;i++)
                    if(*cp32>max_count*(STOP_COUNT-1-i)/STOP_COUNT)
                    {
                        step_count[i]+=*cp32;
                        break;
                    }

                ++cp32;
            }
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
                    chart->DrawCircle(x,y,*cp32);

                ++cp32;
            }
        }
    }

    //统计最大值
    {
        uint32 *cp32=chart->circle_data;

        for(uint i=0;i<width*height;i++)
        {
            if(*cp32>chart->max_count)chart->max_count=*cp32;

            ++cp32;
        }

        std::cout<<"max_count: "<<chart->max_count<<std::endl;
    }

    InitGradient(chart->max_count);

    //生成权重图
    {
        uint32 *cp32=chart->circle_data;
        uint8 *cp8=chart->chart_data;

        float alpha;
        Vector3u8 final_color;

        //1.0   1 0 0
        //0.8   1 1 0
        //0.6   0 1 0
        //0.4   0 1 1
        //0.2   0 0 1
        //0.0   0 0 0

        for(uint i=0;i<width*height;i++)
        {
            alpha=float(*cp32)/float(chart->max_count);

            ColorGradient.Get(final_color,*cp32);

            if(*cp32>0)                 //为了避免最后什么都看不见，所以把没数据的挑出来，剩下的透明度全部加0.25
            {
                alpha+=0.25f;

                if(alpha>1)
                    alpha=1;
            }

            cp8[0]=final_color.b;
            cp8[1]=final_color.g;
            cp8[2]=final_color.r;
            cp8[3]=alpha*255.0f;

            ++cp32;
            cp8+=4;
        }
    }

    if(CHAR_LINE_HEIGHT==0)
        return chart;

    //写入数值
    {
        uint col=10;
        uint row=10;

        AnsiString str;
        AnsiString num_str;
        const AnsiString str_total=AnsiString::numberOf(ps->count);

        AnsiString step_str[6]=
        {
            AnsiString::numberOf(max_count),
            AnsiString::numberOf(uint(max_count*0.8f)),
            AnsiString::numberOf(uint(max_count*0.6f)),
            AnsiString::numberOf(uint(max_count*0.4f)),
            AnsiString::numberOf(uint(max_count*0.2f)),
            AnsiString::numberOf(0),
        };

        char space[32];

        memset(space,' ',32);
        
        str=AnsiString("TOTAL - ")+str_total;

        chart->DrawString(str,col,row,white_color,255);
        row+=CHAR_LINE_HEIGHT;

        chart->DrawGradient(col,row,CHAR_BITMAP_WIDTH*CHAR_BITMAP_SCALE,CHAR_LINE_HEIGHT*STOP_COUNT);

        col+=CHAR_BITMAP_WIDTH*CHAR_BITMAP_SCALE*2;

        for(uint i=0;i<STOP_COUNT;i++)
        {
            str=step_str[i];

            if(i<STOP_COUNT-1)
            {
                num_str=AnsiString::numberOf(step_count[i]);

                str.Strcat(space,str_total.Length()-num_str.Length());

                str+=num_str;

                if(step_count[i]>0)
                {
                    str+=" - ";
                    str+=AnsiString::numberOf(float(step_count[i])*100.0f/float(ps->count));
                    str+="%";
                }
            }

            chart->DrawString(str,col,row,stop_color[i],255);
            row+=CHAR_LINE_HEIGHT;
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

    bool font=InitBitmapFont();

    std::cout<<"file total line: "<<line_count<<std::endl;

    AutoDelete<PositionStat> ps=ToVector2i(sl);

    AutoDelete<Chart> chart=ToChart32(ps);

    {
        util::TGAHeader tga_header;

        util::FillTGAHeader(&tga_header,chart->width,chart->height,4);

        io::OpenFileOutputStream fos(OS_TEXT("chart.tga"),io::FileOpenMode::CreateTrunc);

        if(fos)
        {
            fos->Write(&tga_header,util::TGAHeaderSize);
            fos->Write(chart->chart_data,chart->width*chart->height*4);
            fos->Close();
        }
        else
            std::cerr<<"Create chart.tga failed!"<<std::endl;
    }

    if(font)
       ClearBitmapFont();

    return 0;
}