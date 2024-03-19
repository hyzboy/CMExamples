#include<hgl/util/csv/CSVParse.h>
#include<hgl/type/List.h>
#include<hgl/type/Map.h>
#include<iostream>
#include<hgl/2d/BitmapLoad.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/DrawGeometry.h>
#include"BitmapFont.h"
#include<hgl/color/Color.h>
#include<hgl/filesystem/Filename.h>

using namespace hgl;
using namespace hgl::bitmap;

uint POSITION_SCALE_RATE=100;           //位置缩放比例,unreal中单位为cm
uint64 BATTLE_FIELD_ID=0;               //战场ID

constexpr const uint CHAR_WIDTH=8;
constexpr const uint CHAR_HEIGHT=16;
constexpr const uint CHAR_HALF_WIDTH=4;
constexpr const uint CHAR_HALF_HEIGHT=8;

constexpr const uint ICON_SIZE=8;

BitmapRGB8 *BackgroundBitmap=nullptr;
DrawGeometryRGB8 *draw_bmp=nullptr;

using TraceList=List<Vector2i>;

ObjectMap<uint,TraceList> PlayerTrace;   //玩家轨迹

Vector3u8 *PlayerColor=nullptr;

class TraceParse:public util::CSVParseCallback<char>
{
    uint record_count=0;
    uint parse_count=0;
    uint valid_count=0;

    Vector2i pos;
    uint64 bf_id;
    uint player_id;

public:

    bool ParsePosition(const char *str,const int len)
    {
        const char *sp=str;
        const char *cp;

        cp=hgl::strchr(sp,' ');
        if(!cp)return(false);

        if(!hgl::stoi(sp,pos.x))
            return(false);

        ++cp;
        if(!hgl::stoi(cp,pos.y))
            return(false);

        pos/=POSITION_SCALE_RATE;

        if(pos.x<0||pos.y<0)return(false);
        if(pos.x>=BackgroundBitmap->GetWidth()
         ||pos.y>=BackgroundBitmap->GetHeight())return(false);

        return(true);
    }

    bool OnLine(util::CSVFieldSplite<char> &split) override
    {
        ++record_count;

        const char *p;
        int len;

        p=split.next_field(&len);
        if(!p)
            return(false);

        if(!ParsePosition(p,len))
            return(false);

        p=split.next_field(&len);

        if(!hgl::stou(p,bf_id))
            return(false);

        if(bf_id!=BATTLE_FIELD_ID)
            return(false);

        p=split.next_field(&len);

        if(!hgl::stou(p,player_id))
            return(false);

        ++parse_count;

        {
            TraceList *tl;

            if(!PlayerTrace.Get(player_id,tl))
            {
                tl=new TraceList;

                PlayerTrace.Add(player_id,tl);
            }

            tl->Add(pos);
        }

        return(true);
    }
};//class CSVTest;

bool LoadCSVRecord(const OSString &filename)
{
    TraceParse tp;

    return util::ParseCSVFile(filename,&tp);
}

void DrawIcon(const uint index,const uint x,uint y,const Vector3u8 &color)
{
    const uint8 *sp=Get8x8Char(index);

    if(!sp)return;

    draw_bmp->SetDrawColor(color);
    draw_bmp->DrawMonoBitmap(x,y,sp,8,8);
}

void DrawChar(const char ch,const uint x,const uint y)
{
    const uint8 *sp=Get8x16Char(ch);

    if(!sp)return;

    draw_bmp->DrawMonoBitmap(x,y,sp,CHAR_WIDTH,CHAR_HEIGHT);
}

void DrawString(const uint x,const uint y,const AnsiString &str,const Vector3u8 &color)
{
    const char *sp=str.c_str();
    const uint len=str.Length();
        
    uint pos=x;

    draw_bmp->CloseBlend();

    draw_bmp->SetDrawColor(color);

    for(uint i=0;i<len;i++)
    {
        if(*sp!=' ')
            DrawChar(*sp,pos,y);

        pos+=CHAR_WIDTH;
        ++sp;
    }
}

void StatPlayerTrace()
{
    const uint pc=PlayerTrace.GetCount();
    
    const auto *pt=PlayerTrace.GetDataList();

    uint left=10;
    uint top=10;
    AnsiString str;

    for(uint i=0;i<pc;i++)
    {
        std::cout<<"Player "<<(*pt)->key<<" Trace Count "<<(*pt)->value->GetCount()<<std::endl;

        //draw player icon and id
        {
            DrawIcon(i+1,left,top+4,PlayerColor[i]);

            str=AnsiString::numberOf((*pt)->key);
            DrawString(left+20,top,str,PlayerColor[i]);

            top+=CHAR_HEIGHT+2;

            {
                TraceList *tl=(*pt)->value;

                const Vector2i *last_pos=nullptr;
                const Vector2i *pos=tl->GetData();
                const uint count=tl->GetCount();

                for(uint j=0;j<count;j++)
                {
                    draw_bmp->SetDrawColor(PlayerColor[i]);

                    if(j==0)
                    {
                        draw_bmp->DrawSolidCircle(pos->x,pos->y,8);
                    }
                    else
                    {
                        draw_bmp->DrawLine(last_pos->x,last_pos->y,pos->x,pos->y);                

                        str=AnsiString::numberOf(j);

                        DrawString(pos->x-str.Length()*CHAR_HALF_WIDTH,pos->y-CHAR_HALF_HEIGHT,str,PlayerColor[i]);
                    }

                    last_pos=pos;
                    ++pos;
                }
            }
        }

        ++pt;
    }
}

int os_main(int argc,os_char **argv)
{
    std::cout<<"PlayerTraceChart2D"<<std::endl;

    if(argc<5)
    {
        std::cout<<"Example: map.tga 100 data.csv XXXXXX"<<std::endl;

        std::cout<<"         map.tga    The map image file"<<std::endl;
        std::cout<<"         100        position div rate"<<std::endl;
        std::cout<<"         data.csv   Player trace data file"<<std::endl;
        std::cout<<"         XXXXXX     BattleField ID"<<std::endl;

        return(0);
    }

    {
        BackgroundBitmap=bitmap::LoadBitmapRGB8FromTGA(argv[1]);

        if(!BackgroundBitmap)
        {
            std::cerr<<"Load background bitmap file failed!"<<std::endl;
            return(-1);
        }

        std::cout<<"Load background bitmap file OK!"<<std::endl;

        std::cout<<"Background bitmap size: "<<BackgroundBitmap->GetWidth()<<"x"<<BackgroundBitmap->GetHeight()<<std::endl;
    }

    {
        if(!stou(argv[2],POSITION_SCALE_RATE))
        {
            std::cerr<<"Parse position scale rate failed!"<<std::endl;
            return(-2);
        }

        std::cout<<"Position scale rate is "<<POSITION_SCALE_RATE<<std::endl;
    }

    {
        if(!stou(argv[4],BATTLE_FIELD_ID))
        {
            std::cerr<<"Parse battle field id failed!"<<std::endl;
            return(-3);
        }

        std::cout<<"Battle field id is "<<BATTLE_FIELD_ID<<std::endl;
    }

    {
        if(!LoadBitmapFont())
        {
            std::cerr<<"Load bitmap font failed!"<<std::endl;
            return(-4);
        }
    }

    const OSString csv_filename=argv[3];

    {
        if(!LoadCSVRecord(csv_filename))
        {
            std::cerr<<"Load CSV record file failed!"<<std::endl;
            return(-4);
        }

        std::cout<<"Load CSV record file OK!"<<std::endl;

        {
            uint total=0;
            const uint pc=PlayerTrace.GetCount();
    
            std::cout<<"Player Count: "<<pc<<std::endl;

            if(pc>0)
            {
                PlayerColor=new Vector3u8[pc];

                const auto *pt=PlayerTrace.GetDataList();

                Color3f color;

                for(uint i=0;i<pc;i++)
                {
                    std::cout<<"Player "<<(*pt)->key<<" Trace Count "<<(*pt)->value->GetCount()<<std::endl;

                    GetBGR((COLOR)(i+(int)COLOR::Blue),PlayerColor[i]);

                    total+=(*pt)->value->GetCount();
                    ++pt;
                }
            }

            if(total<=0)return(-5);
        }
    }

    draw_bmp=new DrawGeometryRGB8(BackgroundBitmap);
    StatPlayerTrace();

    {
        const OSString tga_filename=filesystem::ReplaceExtName(csv_filename,OSString(OS_TEXT(".tga")));

        os_out<<OS_TEXT("output: ")<<tga_filename.c_str()<<std::endl;
   
        if(!SaveBitmapToTGA(tga_filename,BackgroundBitmap))
            std::cerr<<"Create tga failed!"<<std::endl;
    }

    delete[] PlayerColor;
    delete draw_bmp;

    ClearBitmapFont();

    return(0);
}