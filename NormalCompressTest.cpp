#include<hgl/graph/NormalData.h>
#include<hgl/util/imgfmt/tga.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/io/FileOutputStream.h>
#include<iostream>

using namespace hgl;
using namespace hgl::filesystem;
using namespace hgl::util;

static uint8 *      file_data=nullptr;
static int64        file_length=0;
static TGAHeader *  tga_header=nullptr;
static uint8 *      bitmap_data=nullptr;
static uint8 *      origin_bitmap_data=nullptr;
static uint         bitmap_bytes=0;
static uint         pixel_count=0;

void SaveTGA(const os_char *filename)
{
    SaveMemoryToFile(filename,file_data,file_length);
}

void TwoBytesNormalTest()
{
    uint8 *two_bytes_normal=new uint8[pixel_count*2];

    Normal3to2(two_bytes_normal,origin_bitmap_data,pixel_count);

    Normal2to3(bitmap_data,two_bytes_normal,pixel_count);

    SaveTGA(OS_TEXT("CompressNormal.tga"));

    {
        uint8 *tp=bitmap_data;
        uint8 *sp=origin_bitmap_data;

        for(uint i=0;i<pixel_count*3;i++)
        {
            *tp=abs(*tp-*sp);
            ++tp;
            ++sp;
        }

        SaveTGA(OS_TEXT("CompressNormalDiff.tga"));
    }

    delete[] two_bytes_normal;
}

int main(int,char **)
{
    file_data=(uint8 *)LoadFileToMemory(OS_TEXT("TestNormal.tga"),file_length);

    if(!file_data)
    {
        std::cerr<<"load file <TestNormal.tga> error!"<<std::endl;
        return(-1);
    }

    tga_header=(TGAHeader *)file_data;

    if(tga_header->bit!=24)
    {
        std::cerr<<"<TestNormal.tga> is not 24bits image!"<<std::endl;
        delete[] file_data;
        return(-1);
    }

    bitmap_data=file_data+sizeof(TGAHeader);

    pixel_count=tga_header->width*tga_header->height;

    bitmap_bytes=pixel_count*3;

    origin_bitmap_data=new uint8[bitmap_bytes];
    memcpy(origin_bitmap_data,bitmap_data,bitmap_bytes);

    TwoBytesNormalTest();

    delete[] origin_bitmap_data;
    delete[] file_data;
    return 0;
}