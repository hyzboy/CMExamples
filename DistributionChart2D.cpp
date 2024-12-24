#include<hgl/type/StringList.h>
#include<hgl/io/LoadStringList.h>
#include<hgl/type/Gradient.h>
#include<hgl/math/Vector.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/filesystem/Filename.h>
#include<iostream>
#include<hgl/2d/BitmapLoad.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/DrawGeometry.h>
#include"BitmapFont.h"

using namespace hgl;
using namespace hgl::bitmap;

OSString csv_filename;

uint POSITION_SCALE_RATE=100;               //坐标缩小比例，UNREAL中单位为厘米，换算到米需要/100。
                                            //原地图4K，现底层为1K，所以需要再/4。
                                            //2K地图用的底层为2K，所以只/100

BitmapRGB8 *BackgroundBitmap=nullptr;

bool LoadBackgroundBitmap()
{
    BackgroundBitmap=bitmap::LoadBitmapRGB8FromTGA(OS_TEXT("mini_map.tga"));

    return(BackgroundBitmap);
}

uint CHAR_BITMAP_WIDTH=0;
uint CHAR_BITMAP_HEIGHT=0;

bool InitBitmapFont()
{
    if(!LoadBitmapFont())
        return(false);

    CHAR_BITMAP_WIDTH=8;
    CHAR_BITMAP_HEIGHT=16;

    return(true);
}

constexpr const float LOW_GAP=0.2f;
constexpr const Vector4u8 black_color={0,0,0,255};
constexpr const Vector4u8 white_color={255,255,255,255};

constexpr const Vector4u8 stop_color[]=
{
    {  0,  0,255,255},
    {  0,255,255,255},
    {  0,255,  0,255},
    {255,255,  0,255},
    {255,  0,  0,255}
};

constexpr const uint STOP_COUNT=sizeof(stop_color)/sizeof(Vector4u8);

uint stop_count[STOP_COUNT];
uint top_count=0;

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

enum class DataSourceType
{
    Error,

    OnePosition,
    TwoPosition,
};

DataSourceType CheckDataSourceType(const U8String &str)
{
    if(str.Length()<=11)return(DataSourceType::Error);

    const char *sp=str.c_str();

    if(*sp=='X')
        return DataSourceType::OnePosition;

    if(hgl::strchr(sp,',',str.Length())!=nullptr)
        return DataSourceType::TwoPosition;

    return(DataSourceType::Error);
}

bool ParsePosition(Vector2i *result,const U8String &str)
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

    result->x/=POSITION_SCALE_RATE;     //Unreal单位为cm,把单位缩到米
    result->y/=POSITION_SCALE_RATE;     //同时把4096的地图缩小到1024

    if(result->x>=BackgroundBitmap->GetWidth()
     ||result->y>=BackgroundBitmap->GetHeight()
     ||result->x<0
     ||result->y<0)
        return(false);

    return(true);
}

using OnePositionData=List<Vector2i>;

struct LineSegment
{
    Vector2i start;
    Vector2i end;
};

using LineSegmentData=List<LineSegment>;

bool ParseLineSegment(LineSegment *result,const U8String &str)
{
    if(!result)return(false);

    if(str.Length()<=0)return(false);

    const char *start=str.c_str();
    const char *end=start+str.Length();
    const char *sp=start;

    if(!hgl::stoi(sp,result->start.x))
        return(false);

    sp=hgl::strchr(sp,' ');
    if(!sp)return(false);

    if(!hgl::stoi(++sp,result->start.y))
        return(false);

    sp=hgl::strchr(sp,',');
    if(!sp)return(false);

    if(!hgl::stoi(++sp,result->end.x))
        return(false);

    sp=hgl::strchr(sp,' ');
    if(!sp)return(false);

    if(!hgl::stoi(++sp,result->end.y))
        return(false);

    result->start.x/=400;
    result->start.y/=400;
    result->end.x/=400;
    result->end.y/=400;
    
    return(true);
}

template<typename T>
void ParseStringList(List<T> &data_list,const U8StringList &sl,bool (*ParseLineFunc)(T *,const U8String &))
{
    const uint count=sl.GetCount();

    data_list.SetCount(count);

    T *p=data_list.GetData();

    U8String str;
    uint result=0;

    for(uint i=0;i<count;i++)
    {
        str=sl[i];

        if(str.Length()<=0)
            continue;

        if(!ParseLineFunc(p,str))
            continue;
      
        ++result;
        ++p;
    }

    data_list.SetCount(result);
}

struct Chart
{
    uint width,height;

    uint max_count;

    BitmapU32 count_bitmap;
    BitmapU32 circle_bitmap;
    BitmapRGBA8 chart_bitmap;

    DrawGeometryU32 *draw_circle=nullptr;
    BlendColorU32Additive blend_u32_additive;

    DrawGeometryRGBA8 *draw_chart=nullptr;
    BlendColorRGBA8 blend_rgba8;

public:

    Chart(const uint w,const uint h)
    {
        width=w;
        height=h;

        count_bitmap.Create(width,height);
        circle_bitmap.Create(width,height);
        chart_bitmap.Create(width,height);

        count_bitmap.ClearColor(0);
        circle_bitmap.ClearColor(0);
        chart_bitmap.ClearColor(black_color);

        draw_circle=new DrawGeometryU32(&circle_bitmap);
        draw_circle->SetBlend(&blend_u32_additive);

        draw_chart=new DrawGeometryRGBA8(&chart_bitmap);
        draw_chart->SetBlend(&blend_rgba8);

        max_count=0;
    }

    ~Chart()
    {
        delete draw_chart;
        delete draw_circle;
    }

    void DrawCircle(uint x,uint y,uint radius)
    {
        draw_circle->SetDrawColor(1);
        draw_circle->DrawSolidCircle(x,y,radius);
    }

private:

    void DrawChar(const char ch,const uint x,const uint y)
    {
        const uint8 *sp=Get8x16Char(ch);

        if(!sp)return;

        draw_chart->DrawMonoBitmap(x,y,sp,CHAR_BITMAP_WIDTH,CHAR_BITMAP_HEIGHT);
    }

public:

    void DrawString(const AnsiString &str,const uint x,const uint y,const Vector3u8 &stop_color)
    {
        const char *sp=str.c_str();
        const uint len=str.Length();
        
        uint pos=x;

        draw_chart->CloseBlend();

        draw_chart->SetDrawColor(Vector4u8(stop_color,255));

        for(uint i=0;i<len;i++)
        {
            if(*sp!=' ')
                DrawChar(*sp,pos,y);

            pos+=CHAR_BITMAP_WIDTH;
            ++sp;
        }
    }

    void DrawGradient(const uint left,const uint top,const uint w,const uint h)
    {
        Vector3u8 rgb;
        Vector4u8 color;

        uint low,high,gap;
        
        ColorGradient.GetLowestPosition(low);
        ColorGradient.GetHighestPosition(high);

        gap=high-low;

        draw_chart->CloseBlend();

        for(uint i=0;i<h;i++)
        {
            ColorGradient.Get(rgb,(1.0f-float(i)/float(h))*float(gap)+float(low));

            color=Vector4u8(rgb,255);

            draw_chart->SetDrawColor(color);
            draw_chart->DrawHLine(left,top+i,w);
        }
    }
};

Chart *CreateChart()
{
    const uint width=BackgroundBitmap->GetWidth();
    const uint height=BackgroundBitmap->GetHeight();

    std::cout<<"width: "<<width<<",height: "<<height<<std::endl;    

    return(new Chart(width,height));
}

void StatData(BitmapU32 &count_bitmap,const OnePositionData &opd)
{    
    top_count=0;

    //统计每个格子数据数量
    {
        uint32 *cp32;

        const Vector2i *p=opd.GetData();

        for(int i=0;i<opd.GetCount();i++)
        {
            cp32=count_bitmap.GetData(p->x,p->y);

            ++(*cp32);

            if(*cp32>top_count)top_count=*cp32;

            ++p;
        }
    }
}

void StatData(BitmapU32 &count_bitmap,const LineSegmentData &lsd)
{
    {
        BlendColorU32Additive blend_u32_additive;
        DrawGeometryU32 draw_bitmap(&count_bitmap);
        draw_bitmap.SetBlend(&blend_u32_additive);

        draw_bitmap.SetDrawColor(1);

        const LineSegment *p=lsd.GetData();

        for(int i=0;i<lsd.GetCount();i++)
        {
            draw_bitmap.DrawLine(p->start.x,p->start.y,p->end.x,p->end.y);

            ++p;
        }
    }

    {
        top_count=0;

        uint32 *cp32=count_bitmap.GetData();

        for(uint i=0;i<count_bitmap.GetTotalPixels();i++)
        {
            if(*cp32>top_count)top_count=*cp32;

            ++cp32;
        }
    }
}

void StatStopCount(const BitmapU32 &count_bitmap)
{
    //统计占比
    {
        const uint32 *cp32=count_bitmap.GetData();

        hgl_zero(stop_count);

        for(uint i=0;i<count_bitmap.GetTotalPixels();i++)
        {
            if(*cp32>0)
            for(uint i=0;i<STOP_COUNT;i++)
                if(*cp32>top_count*(STOP_COUNT-1-i)/STOP_COUNT)
                {
                    stop_count[i]+=*cp32;
                    break;
                }

            ++cp32;
        }
    }
}

void CountToCircle(Chart *chart)
{
    const uint32 *cp32=chart->count_bitmap.GetData();

    const uint width=chart->width;
    const uint height=chart->height;

    for(uint y=0;y<height;y++)
    {
        for(uint x=0;x<width;x++)
        {
            if(*cp32>0)
                chart->DrawCircle(x,y,(*cp32));

            ++cp32;
        }
    }
}

void ChartStat(Chart *chart,const uint data_count)
{
    const uint width=chart->width;
    const uint height=chart->height;

    //统计最大值
    {
        uint32 *cp32=chart->circle_bitmap.GetData();

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
        uint32 *cp32=chart->circle_bitmap.GetData();
        Vector4u8 *cp8=chart->chart_bitmap.GetData();

        float alpha;
        Vector3u8 final_color;

        for(uint i=0;i<width*height;i++)
        {
            alpha=float(*cp32)/float(chart->max_count);

            ColorGradient.Get(final_color,*cp32);

            if(*cp32>0)                 //为了避免最后什么都看不见，所以把没数据的挑出来，剩下的透明度全部加0.25
            {
                alpha+=LOW_GAP;

                if(alpha>1)
                    alpha=1;
            }

            cp8->b=final_color.b;
            cp8->g=final_color.g;
            cp8->r=final_color.r;
            cp8->a=alpha*255.0f;

            ++cp32;
            ++cp8;
        }
    }

    if(CHAR_BITMAP_HEIGHT==0)
        return;

    //写入数值
    {
        int col=10;
        int row=10;
        int stop_str_width=0;

        AnsiString str;
        AnsiString num_str;
        const AnsiString str_total=AnsiString::numberOf(data_count);

        AnsiString step_str[STOP_COUNT];
        const uint dradient_bar_height=CHAR_BITMAP_HEIGHT*STOP_COUNT;

        char space[32];

        memset(space,' ',32);

        for(uint i=0;i<STOP_COUNT;i++)
        {
            step_str[i]=AnsiString::numberOf(uint(top_count*(1.0-float(i)/float(STOP_COUNT))));

            if(stop_str_width<step_str[i].Length())
                stop_str_width=step_str[i].Length();
        }

        str="Source: "+ToAnsiString(csv_filename);

        chart->DrawString(str,col,row,black_color);

        row+=CHAR_BITMAP_HEIGHT*2;
        
        str=AnsiString("Total: ")+str_total;

        chart->DrawString(str,col,row,white_color);
        row+=CHAR_BITMAP_HEIGHT;

        chart->DrawGradient(col,row,CHAR_BITMAP_WIDTH,dradient_bar_height);

        col+=CHAR_BITMAP_WIDTH*2;

        chart->DrawGradient(col+(stop_str_width+1)*CHAR_BITMAP_WIDTH,
                            row,CHAR_BITMAP_WIDTH,dradient_bar_height);

        chart->DrawGradient(col+(str_total.Length()+stop_str_width+4)*CHAR_BITMAP_WIDTH,
                            row,CHAR_BITMAP_WIDTH,dradient_bar_height);

        for(uint i=0;i<STOP_COUNT;i++)
        {
            str.Strcpy(space,stop_str_width-step_str[i].Length());

            str+=step_str[i];

            num_str=AnsiString::numberOf(stop_count[i]);

            str.Strcat(space,str_total.Length()-num_str.Length()+3);

            str+=num_str;

            if(stop_count[i]>0)
            {
                num_str=AnsiString::floatOf(float(stop_count[i])*100.0f/float(data_count),4);

                str.Strcat(space,3+(8-num_str.Length()));
                str+=num_str;
                str+="%";
            }

            chart->DrawString(str,col,row,stop_color[i]);
            row+=CHAR_BITMAP_HEIGHT;
        }
    }

    //混合底图
    if(BackgroundBitmap)
    {
        BlendBitmapRGBA8toRGB8 blend;

        blend(&(chart->chart_bitmap),BackgroundBitmap,1.0);
    }
}

int os_main(int argc,os_char **argv)
{
    std::cout<<"Distribution Chart 2D"<<std::endl<<std::endl;

    if(argc<2)
    {
        std::cout<<"example: DistributionChart2D data.csv"<<std::endl<<std::endl;
        return 0;
    }

    if(!LoadBackgroundBitmap())
    {
        std::cerr<<"can't load background mini_map.tga !"<<std::endl;
        return 1;
    }

    if(!InitBitmapFont())
    {
        std::cerr<<"can't load font file VGA8.F16 !"<<std::endl;
        return 2;
    }

    U8StringList sl;

    csv_filename=argv[1];

    int line_count=LoadStringListFromTextFile(sl,csv_filename);
    
    if(line_count<=1)
    {
        os_out<<OS_TEXT("Load file ")<<csv_filename.c_str()<<OS_TEXT(" failed!")<<std::endl;
        return(3);
    }

    os_out<<OS_TEXT("Load file ")<<csv_filename.c_str()<<OS_TEXT(" OK!")<<std::endl;

    std::cout<<"file total line: "<<line_count<<std::endl;

    const DataSourceType dst=CheckDataSourceType(sl[0]);

    if(dst==DataSourceType::Error)
    {
        os_out<<OS_TEXT("Check data source type failed!")<<std::endl;
        return(4);
    }

    AutoDelete<Chart> chart=CreateChart();
    uint data_count;

    if(dst==DataSourceType::OnePosition)
    {
        os_out<<OS_TEXT("Data source type: One Position")<<std::endl;

        OnePositionData opd;

        ParseStringList<Vector2i>(opd,sl,ParsePosition);

        StatStopCount(chart->count_bitmap);

        StatData(chart->count_bitmap,opd);

        CountToCircle(chart);

        data_count=opd.GetCount();
    }
    else
    {
        os_out<<OS_TEXT("Data source type: Two Position")<<std::endl;

        LineSegmentData lsd;

        ParseStringList<LineSegment>(lsd,sl,ParseLineSegment);

        StatStopCount(chart->circle_bitmap);

        StatData(chart->circle_bitmap,lsd);

        data_count=lsd.GetCount();        
    }

    ChartStat(chart,data_count);

    {
        const OSString tga_filename=filesystem::ReplaceExtName(csv_filename,OSString(OS_TEXT(".tga")));

        os_out<<OS_TEXT("output: ")<<tga_filename.c_str()<<std::endl;
   
        if(!SaveBitmapToTGA(tga_filename,BackgroundBitmap))
            std::cerr<<"Create chart.tga failed!"<<std::endl;
    }

    delete BackgroundBitmap;
    ClearBitmapFont();

    return 0;
}
