#include<hgl/filesystem/FileSystem.h>

using namespace hgl;

namespace
{
    uint8 *bitmap_font_data=nullptr;
}//namespace

bool LoadBitmapFont()
{
    if(bitmap_font_data)
        return(true);

    bitmap_font_data=new uint8[256*16];

    if(!filesystem::LoadFileToMemory(OS_TEXT("VGA8.F16"), (void **)&bitmap_font_data))
        return(false);

    return(true);
}

void ClearBitmapFont()
{
    SAFE_CLEAR_ARRAY(bitmap_font_data);
}

const uint GetCharWidth(){return bitmap_font_data?8:0;}
const uint GetCharHeight(){return bitmap_font_data?16:0;}

const uint8 *GetBitmapChar(const char ch)
{
    return bitmap_font_data+uchar(ch)*16;
}