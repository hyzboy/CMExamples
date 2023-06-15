#include<iostream>
#include<iomanip>
#include<hgl/text/FontInfo.h>
#include<hgl/util/json/JsonTool.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/io/TextOutputStream.h>

using namespace hgl;

constexpr const os_char font_type_string[3][10]=
{
    OS_TEXT("Raster"),
    OS_TEXT("Vector"),
    OS_TEXT("TrueType")
};

#define JSON_OUT_FI_UINT(name)   cur_fi[#name]=(uint)(fi->name);
#define JSON_OUT_FI_BOOL(name)   cur_fi[#name]=(fi->name?"true":"false");

void OutputJson(const FontInfoList &fi_list)
{
    const uint count=fi_list.GetCount();
    FontInfo *fi=fi_list.GetData();

    Json::Value root;
    Json::Value font_list;

    root["count"]=count;

    for(uint i=0;i<count;i++)
    {
        Json::Value cur_fi;

        cur_fi["index"]=i;
        cur_fi["name"]=to_u8(fi->name).c_str();
        cur_fi["type"]=to_u8(font_type_string[(size_t)fi->type]).c_str();

        JSON_OUT_FI_UINT(charset)
        JSON_OUT_FI_UINT(height)
        JSON_OUT_FI_UINT(ascent)
        JSON_OUT_FI_UINT(descent)
        JSON_OUT_FI_UINT(internal_leading)
        JSON_OUT_FI_UINT(external_leading)
        JSON_OUT_FI_UINT(ave_char_width)
        JSON_OUT_FI_UINT(max_char_width)
        JSON_OUT_FI_UINT(weight)
        JSON_OUT_FI_UINT(overhang)
        JSON_OUT_FI_UINT(digitized_aspect_x)
        JSON_OUT_FI_UINT(digitized_aspect_y)
        JSON_OUT_FI_UINT(first_char)
        JSON_OUT_FI_UINT(last_char)
        JSON_OUT_FI_UINT(default_char)
        JSON_OUT_FI_UINT(break_char)
        JSON_OUT_FI_BOOL(italic)
        JSON_OUT_FI_BOOL(underlined)
        JSON_OUT_FI_BOOL(struck_out)
        JSON_OUT_FI_UINT(pitch_and_family)

        font_list.append(cur_fi);

        ++fi;
    }

    root["font_list"]=font_list;

    OSString error_info;
    SaveJson(root,OS_TEXT("FontList.json"),error_info);
}

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

void main()
{
    FontInfoList fi_list;

    EnumOSFonts(&fi_list);

    const uint count=fi_list.GetCount();

    std::cout<<"os had "<<count<<" fonts"<<std::endl<<std::endl;

    OutputJson(fi_list);
    OutputCSV(fi_list);
}