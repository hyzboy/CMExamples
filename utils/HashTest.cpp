#include<hgl/util/hash/Hash.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/Time.h>
#include<iostream>
#include<iomanip>

using namespace std;
using namespace hgl;
using namespace hgl::util;

int os_main(int argc,os_char **argv)
{
    if(argc<2)
    {
        cout<<"CountHash [filename]"<<endl;
        return(0);
    }

    os_out<<OS_TEXT("CountHash \"")<<argv[1]<<OS_TEXT('"')<<endl;

    void *file_data;
    int64 file_length;

    file_length=filesystem::LoadFileToMemory(argv[1],&file_data);

    cout<<"file length: "<<file_length<<endl;

    UTF8String hash_name;
    int hash_length;
    char *hash_code;
    char hash_str[256];

    double start_time,end_time;

    ENUM_CLASS_FOR(HASH,int,i)
    {
        Hash *h=CreateHash(HASH(i));
        hash_length=h->GetHashBytes();

        hash_code=new char[hash_length];

        start_time=GetDoubleTime();

        h->Init();
        h->Update(file_data,file_length);
        h->Final(hash_code);

        end_time=GetDoubleTime();

        ToLowerHexStr(hash_str,hash_code,hash_length);

        hash_str[hash_length*2]=0;

        h->GetName(hash_name);

        cout<<setw(8)<<hash_name.c_str()<<":"<<hash_str<<",use time: "<<(end_time-start_time)<<endl;

        delete[] hash_code;
        delete h;
    }
}
