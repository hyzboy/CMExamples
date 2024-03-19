#include<hgl/filesystem/FileSystem.h>

using namespace hgl;

namespace
{
    uint8 *bitmap_font_8x16_data=nullptr;
    uint8 *bitmap_font_8x8_data=nullptr;
}//namespace

void ClearBitmapFont()
{
    SAFE_CLEAR_ARRAY(bitmap_font_8x16_data);
    SAFE_CLEAR_ARRAY(bitmap_font_8x8_data);
}

bool LoadBitmapFont()
{
    ClearBitmapFont();

    filesystem::LoadFileToMemory(OS_TEXT("VGA8.F16"), (void **)&bitmap_font_8x16_data);
    filesystem::LoadFileToMemory(OS_TEXT("VGA8.F8"), (void **)&bitmap_font_8x8_data);

    return(true);
}

const uint8 *Get8x16Char(const char ch)
{
    return bitmap_font_8x16_data+uchar(ch)*16;
}

const uint8 *Get8x8Char(const char ch)
{
    return bitmap_font_8x8_data+uchar(ch)*8;
}