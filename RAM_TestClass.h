#pragma once
#include"RuntimeAssetManager.h"
#include<hgl/type/SortedSets.h>

using namespace hgl;

using InstanceID        =uint64_t;
using PhysicalDeviceID  =uint64_t;

struct Instance:public RuntimeAsset<InstanceID,Instance>
{
    SortedSets<PhysicalDeviceID> physical_devices;

public:

    using RuntimeAsset::RuntimeAsset;

    void AddPhysicalDevice(PhysicalDeviceID pd_id)
    {
        physical_devices.Add(pd_id);
    }

    const SortedSets<PhysicalDeviceID> &GetPhysicalDevices()const{return physical_devices;}
};

struct PhysicalDevice:public RuntimeAsset<PhysicalDeviceID,PhysicalDevice>
{
    InstanceID inst_id;

    AnsiString device_name;

public:

    using RuntimeAsset::RuntimeAsset;

    bool Init(const AnsiString &name,const InstanceID &iid)
    {
        device_name=name;
        inst_id=iid;

        return(true);
    }
};
