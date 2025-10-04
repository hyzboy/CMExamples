#include<iostream>
#include<iomanip>
#include<hgl/text/FontInfo.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/io/TextOutputStream.h>

using namespace hgl;

constexpr const os_char font_type_string[3][10]=
{
    OS_TEXT("Raster"),
    OS_TEXT("Vector"),
    OS_TEXT("TrueType")
};

void OutputCSV(const FontInfoList &fi_list)
{
    io::FileOutputStream fos;

    if(!fos.CreateTrunc(OS_TEXT("FontList.csv")))
        return;

    io::TextOutputStream *tos=new io::UTF16LETextOutputStream(&fos);

    tos->WriteBOM();

    OSString str=OS_TEXT("index,name,type,charset,height,ascent,descent,internal_leading,external_leading,ave_char_width,max_char_width,weight,overhang,digitized_aspect_x,digitized_aspect_y,first_char,last_char,default_char,break_char,italic,underlined,struck_out,pitch_and_family");

    tos->WriteLine(str);

    const uint count=fi_list.GetCount();
    FontInfo *fi=fi_list.GetData();

    for(uint i=0;i<count;i++)
    {
        str=OSString::numberOf(i)+OS_TEXT(",\"")+
            OSString(fi->name)+OS_TEXT("\",\"")+
            OSString(font_type_string[(size_t)fi->type])+OS_TEXT("\",")+
            OSString::numberOf(fi->charset)+OS_TEXT(",")+
            OSString::numberOf(fi->height)+OS_TEXT(",")+
            OSString::numberOf(fi->ascent)+OS_TEXT(",")+
            OSString::numberOf(fi->descent)+OS_TEXT(",")+
            OSString::numberOf(fi->internal_leading)+OS_TEXT(",")+
            OSString::numberOf(fi->external_leading)+OS_TEXT(",")+
            OSString::numberOf(fi->ave_char_width)+OS_TEXT(",")+
            OSString::numberOf(fi->max_char_width)+OS_TEXT(",")+
            OSString::numberOf(fi->weight)+OS_TEXT(",")+
            OSString::numberOf(fi->overhang)+OS_TEXT(",")+
            OSString::numberOf(fi->digitized_aspect_x)+OS_TEXT(",")+
            OSString::numberOf(fi->digitized_aspect_y)+OS_TEXT(",")+
            OSString::numberOf(fi->first_char)+OS_TEXT(",")+
            OSString::numberOf(fi->last_char)+OS_TEXT(",")+
            OSString::numberOf(fi->default_char)+OS_TEXT(",")+
            OSString::numberOf(fi->break_char)+OS_TEXT(",")+
            OSString::numberOf(fi->italic)+OS_TEXT(",")+
            OSString::numberOf(fi->underlined)+OS_TEXT(",")+
            OSString::numberOf(fi->struck_out)+OS_TEXT(",")+
            OSString::numberOf(fi->pitch_and_family);

        tos->WriteLine(str);
        ++fi;
    }

    delete tos;
    fos.Close();
}

int os_main(int,os_char **)
{
    FontInfoList fi_list;

    EnumOSFonts(&fi_list);

    const uint count=fi_list.GetCount();

    std::cout<<"os had "<<count<<" fonts"<<std::endl<<std::endl;

    OutputCSV(fi_list);
    
    return 0;
}