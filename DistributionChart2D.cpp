#include<hgl/type/StringList.h>
#include<hgl/type/LoadStringList.h>
#include<hgl/type/Gradient.h>
#include<hgl/math/Vector.h>
#include<hgl/util/imgfmt/tga.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/filesystem/Filename.h>
#include<iostream>
#include<hgl/2d/Bitmap.h>
#include<hgl/2d/DrawGeometry.h>
#include"BitmapFont.h"

using namespace hgl;

OSString csv_filename;

using BitmapRGB8=bitmap::Bitmap<Vector3u8>;
using BitmapRGBA8=bitmap::Bitmap<Vector4u8>;

using BitmapU32=bitmap::Bitmap<uint32>;
using DrawBitmapU32=bitmap::DrawGeometry<uint32>;

using DrawBitmapRGBA8=bitmap::DrawGeometry<Vector4u8>;

struct BlendColorU32Additive:public bitmap::BlendColor<uint32>
{
    const uint32 operator()(const uint32 &src,const uint32 &dst)const
    {
        uint64 result=src+dst;

        return (result>HGL_U32_MAX)?HGL_U32_MAX:(result&HGL_U32_MAX);
    }

    const uint32 operator()(const uint32 &src,const uint32 &dst,const float &alpha)const
    {
        uint64 result=src*alpha+dst;

        return (result>HGL_U32_MAX)?HGL_U32_MAX:(result&HGL_U32_MAX);
    }
};

struct BlendColorRGBA8:public bitmap::BlendColor<Vector4u8>
{
    const Vector4u8 operator()(const Vector4u8 &src,const Vector4u8 &dst)const
    {
        uint8 na=255-src.a;

        return Vector4u8((src.r*src.a+dst.r*na)/255,
                         (src.g*src.a+dst.g*na)/255,
                         (src.b*src.a+dst.b*na)/255,
                         dst.a);
    }

    const Vector4u8 operator()(const Vector4u8 &src,const Vector4u8 &dst,const float &alpha)const
    {
        uint8 a=src.a*alpha;
        uint8 na=255-src.a;

        return Vector4u8((src.r*src.a+dst.r*na)/255,
                         (src.g*src.a+dst.g*na)/255,
                         (src.b*src.a+dst.b*na)/255,
                         dst.a);
    }
};

BitmapRGB8 *BackgroundBitmap=nullptr;

bool LoadBackgroundBitmap()
{
    io::OpenFileInputStream fis(OS_TEXT("mini_map.tga"));

    if(!fis)
        return(false);

    util::TGAHeader tga_header;
    util::TGAImageDesc tga_desc;

    fis->Read(&tga_header,sizeof(util::TGAHeader));

    if(tga_header.image_type!=util::TGA_IMAGE_TYPE_TRUE_COLOR)
        return(false);

    if(tga_header.bit!=24)
        return(false);

    tga_desc.image_desc=tga_header.image_desc;

    BackgroundBitmap=new BitmapRGB8;

    BackgroundBitmap->Create(tga_header.width,tga_header.height);

    const uint total_bytes=BackgroundBitmap->GetTotalBytes();

    if(fis->Read(BackgroundBitmap->GetData(),total_bytes)!=total_bytes)
    {
        delete BackgroundBitmap;
        BackgroundBitmap=nullptr;
        return(false);
    }

    if(tga_desc.direction==util::TGA_DIRECTION_LOWER_LEFT)
       BackgroundBitmap->Flip();

    return(true);
}

uint CHAR_BITMAP_WIDTH=0;
uint CHAR_BITMAP_HEIGHT=0;

bool InitBitmapFont()
{
    if(!LoadBitmapFont())
        return(false);

    CHAR_BITMAP_WIDTH=GetCharWidth();
    CHAR_BITMAP_HEIGHT=GetCharHeight();

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

    for(uint i=0;i<count;i++)
    {
        str=sl[i];

        if(str.Length()<=0)
            continue;

        if(!ParseLine(p,str))
            continue;

        (*p)/=100;      //Unreal单位为cm,把单位缩到米
        (*p)/=4;

        if(p->x>=BackgroundBitmap->GetWidth()
         ||p->y>=BackgroundBitmap->GetHeight())
            continue;

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

    BitmapU32 count_bitmap;
    BitmapU32 circle_bitmap;
    BitmapRGBA8 chart_bitmap;

    DrawBitmapU32 *draw_circle=nullptr;
    BlendColorU32Additive blend_u32_additive;

    DrawBitmapRGBA8 *draw_chart=nullptr;
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

        draw_circle=new DrawBitmapU32(&circle_bitmap);
        draw_circle->SetBlend(&blend_u32_additive);

        draw_chart=new DrawBitmapRGBA8(&chart_bitmap);
        draw_chart->SetBlend(&blend_rgba8);

        max_count=0;
    }

    ~Chart()
    {
        delete draw_circle;
    }

    void DrawCircle(uint x,uint y,uint radius)
    {
        draw_circle->SetDrawColor(1);
        draw_circle->DrawSolidCircle(x,y,radius);
    }

    void DrawChar(const char ch,const uint x,const uint y,const Vector3u8 &stop_color,const uint8 alpha)
    {
        const uint8 *sp=GetBitmapChar(ch);
        uint8 bit;
        Vector4u8 *tp=chart_bitmap.GetData(x,y);

        const uint line_wrap=width-CHAR_BITMAP_WIDTH;

        for(uint row=0;row<CHAR_BITMAP_HEIGHT;row++)
        {
            bit=1<<7;

            for(uint col=0;col<CHAR_BITMAP_WIDTH;col++)
            {
                if(*sp&bit)
                {
                    tp->r=stop_color.r;
                    tp->g=stop_color.g;
                    tp->b=stop_color.b;
                    tp->a=255;
                }

                ++tp;

                bit>>=1;
            }

            tp+=line_wrap;
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

            pos+=CHAR_BITMAP_WIDTH;
            ++sp;
        }
    }

    void DrawGradient(const uint left,const uint top,const uint w,const uint h)
    {
        Vector3u8 rgb;
        Vector4u8 color;

        uint low,high,gap;
        
        ColorGradient.GetLowest(low);
        ColorGradient.GetHighest(high);

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

Chart *ToChart32(const PositionStat *ps)
{
    const uint width=BackgroundBitmap->GetWidth();
    const uint height=BackgroundBitmap->GetHeight();

    std::cout<<"width: "<<width<<",height: "<<height<<std::endl;    

    Chart *chart=new Chart(width,height);
    uint max_count=0;
    uint step_count[STOP_COUNT];

    //统计每个格子数据数量
    {
        uint x,y;
        uint32 *cp32;

        const Vector2i *p=ps->data;

        for(uint i=0;i<ps->count;i++)
        {
            x=p->x-ps->minp.x;
            y=p->y-ps->minp.y;

            cp32=chart->count_bitmap.GetData(x,y);

            ++(*cp32);

            if(*cp32>max_count)max_count=*cp32;

            ++p;
        }
    }

    //统计占比
    {
        uint32 *cp32=chart->count_bitmap.GetData();

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
        uint32 *cp32=chart->count_bitmap.GetData();

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
        return chart;

    //写入数值
    {
        int col=10;
        int row=10;
        int stop_str_width=0;

        AnsiString str;
        AnsiString num_str;
        const AnsiString str_total=AnsiString::numberOf(ps->count);

        AnsiString step_str[STOP_COUNT];
        const uint dradient_bar_height=CHAR_BITMAP_HEIGHT*STOP_COUNT;

        char space[32];

        memset(space,' ',32);

        for(uint i=0;i<STOP_COUNT;i++)
        {
            step_str[i]=AnsiString::numberOf(uint(max_count*(1.0-float(i)/float(STOP_COUNT))));

            if(stop_str_width<step_str[i].Length())
                stop_str_width=step_str[i].Length();
        }

        str="Source: "+ToAnsiString(csv_filename);

        chart->DrawString(str,col,row,black_color,255);

        row+=CHAR_BITMAP_HEIGHT*2;
        
        str=AnsiString("Total: ")+str_total;

        chart->DrawString(str,col,row,white_color,255);
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

            num_str=AnsiString::numberOf(step_count[i]);

            str.Strcat(space,str_total.Length()-num_str.Length()+3);

            str+=num_str;

            if(step_count[i]>0)
            {
                num_str=AnsiString::floatOf(float(step_count[i])*100.0f/float(ps->count),4);

                str.Strcat(space,3+(8-num_str.Length()));
                str+=num_str;
                str+="%";
            }

            chart->DrawString(str,col,row,stop_color[i],255);
            row+=CHAR_BITMAP_HEIGHT;
        }
    }

    //混合底图
    if(BackgroundBitmap)
    {
        Vector4u8 *p=chart->chart_bitmap.GetData();
        Vector3u8 *bp=BackgroundBitmap->GetData();
        uint8 alpha;

        for(uint row=0;row<height;row++)
        {
            for(uint col=0;col<width;col++)
            {
                alpha=p->a;

                p->r=(p->r*alpha+bp->r*(255-alpha))/255;
                p->g=(p->g*alpha+bp->g*(255-alpha))/255;
                p->b=(p->b*alpha+bp->b*(255-alpha))/255;
                p->a=255;

                ++p;
                ++bp;
            }
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

    UTF8StringList sl;

    csv_filename=argv[1];

    int line_count=LoadStringListFromTextFile(sl,csv_filename);
    
    if(line_count<=1)
    {
        os_out<<OS_TEXT("Load file ")<<csv_filename.c_str()<<OS_TEXT(" failed!")<<std::endl;
        return(3);
    }

    os_out<<OS_TEXT("Load file ")<<csv_filename.c_str()<<OS_TEXT(" OK!")<<std::endl;

    std::cout<<"file total line: "<<line_count<<std::endl;

    AutoDelete<PositionStat> ps=ToVector2i(sl);

    AutoDelete<Chart> chart=ToChart32(ps);

    OSString tga_filename;
    {
        tga_filename=filesystem::ReplaceExtName(csv_filename,OSString(OS_TEXT(".tga")));

        os_out<<OS_TEXT("output: ")<<tga_filename.c_str()<<std::endl;
    }

    {
        util::TGAHeader tga_header;

        util::FillTGAHeader(&tga_header,chart->width,chart->height,4);

        io::OpenFileOutputStream fos(tga_filename.c_str(),io::FileOpenMode::CreateTrunc);

        if(fos)
        {
            fos->Write(&tga_header,util::TGAHeaderSize);
            fos->Write(chart->chart_bitmap.GetData(),chart->chart_bitmap.GetTotalBytes());
            fos->Close();
        }
        else
            std::cerr<<"Create chart.tga failed!"<<std::endl;
    }

    delete BackgroundBitmap;
    ClearBitmapFont();

    return 0;
}
