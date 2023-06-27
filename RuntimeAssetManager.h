#pragma once

#include<hgl/type/ResManage.h>

using namespace hgl;

template<typename K,typename V> struct RuntimeAssetManager:public ResManage<K,V>
{
public:

    bool Add(V *v)
    {
        if(!v)return(false);

        return ResManage<K,V>::Add(v->GetID(),v);
    }
};

template<typename K,typename V> struct RuntimeAsset
{
public:

    using RAMClass=RuntimeAssetManager<K,V>;

private:

    static RuntimeAssetManager<K,V> RAM;

private:

    K RuntimeAssetID;

public:

    const K GetID()const{return RuntimeAssetID;}

public:

    RuntimeAsset(K id)
    {
        RuntimeAssetID=id;
    }

    virtual ~RuntimeAsset()=default;

    bool Init()
    {
        return(true);
    }

public:

    static uint GetInstanceCount()
    {
        return RAM.GetCount();
    }

    static RuntimeAssetManager<K,V> &GetRAM()
    {
        return RAM;
    }

    template<typename ...ARGS>
    static V *CreateInstance(const K &id,ARGS...args)
    {
        V *obj=new V(id);

        if(!obj)return(nullptr);

        if(!obj->Init(args...))
        {
            delete obj;
            return(nullptr);
        }

        RAM.Add(obj);
        return obj;
    }

    static V *GetInstance(const K &id)
    {
        return RAM.Get(id);
    }

    void ReleaseInstance()
    {
        RAM.Release(RuntimeAssetID);
        delete this;
    }
};

#define HGL_RUNTIME_ASSET_DECLARATION(RA_ID_TYPE,RA_CLASS)  RuntimeAssetManager<RA_ID_TYPE,RA_CLASS> RuntimeAsset<RA_ID_TYPE,RA_CLASS>::RAM;
