#include<hgl/util/csv/CSVParse.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/platform/SOC.h>
#include<hgl/type/Map.h>
#include<iostream>

using namespace hgl;
using namespace hgl::util;
using namespace hgl::filesystem;

namespace
{
    Map<AnsiString,SOCProductInfo> soc_product_map;

    class SOCProductInfoParse:public CSVParseCallback<char>
    {
        uint record_count=0;
        uint parse_count=0;
        uint valid_count=0;

        SOCProductInfo soc;

    public:

        ~SOCProductInfoParse()
        {
            std::cout<<"A total of "<<record_count<<" records, successful "<<parse_count<<", valid record "<<valid_count<<std::endl;
        }

        bool OnLine(util::CSVFieldSplite<char> &split) override
        {
            ++record_count;

            if(record_count<=2) //前2行是标题,跳过
                return(true);

            hgl_zero(soc);

            const char *p;
            int len;
        
            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                soc.soc_info.vendor=ParseSOCVendor(p);
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                hgl::strcpy(soc.soc_info.model,sizeof(soc.soc_info.model),p,len);
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                hgl::strcpy(soc.product_name,sizeof(soc.product_name),p,len);
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                //release time,discard
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                soc.gpu_info.vendor=ParseSOCGPUVendor(p);
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                hgl::strcpy(soc.gpu_info.model,sizeof(soc.gpu_info.model),p,len);
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                hgl::stou(p,len,soc.gpu_info.core_count);
            }

            {
                p=split.next_field(&len);
                if(!p)
                    return(false);

                hgl::stou(p,len,soc.gpu_info.freq);
            }

            for(uint i=0;i<4;i++)
            {
                p=split.next_field(&len);

                if(len<=0)
                {
                    split.next_field(&len);   //skip core count
                    split.next_field(&len);   //skip freq

                    continue;
                }

                hgl::strcpy(soc.cpu_cluster[soc.cpu_cluster_count].core,sizeof(soc.cpu_cluster[soc.cpu_cluster_count].core),p,len);

                p=split.next_field(&len);
                if(!p)
                    return(false);

                hgl::stou(p,len,soc.cpu_cluster[soc.cpu_cluster_count].count);

                p=split.next_field(&len);
                if(p)
                    hgl::stof(p,len,soc.cpu_cluster[soc.cpu_cluster_count].freq);

                ++soc.cpu_cluster_count;
            }

            soc_product_map.Add(AnsiString(soc.soc_info.model),soc);
            return(true);
        }
    };//class SOCProductInfoParse:public CSVParseCallback<char>
}//namespace

const SOCProductInfo *GetSOCProductInfo(const AnsiString &name)
{
    return soc_product_map.GetPointer(name);
}

bool LoadSOCInfo(const OSString &path)
{
    const OSString filename=MergeFilename(path,OS_TEXT("SOC.csv"));

    if(!FileExist(filename))
    {
        os_out<<"the file \""<<filename.c_str()<<"\" don't exist!"<<std::endl;

        return(false);
    }

    os_out<<"start to load SOC DataBase from \""<<filename.c_str()<<"\". "<<std::endl;

    SOCProductInfoParse parse;

    return ParseCSVFile(filename,&parse);
}
