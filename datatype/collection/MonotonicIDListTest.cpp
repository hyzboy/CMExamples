#include<hgl/type/DataArray.h>
#include<hgl/type/SortedSet.h>
#include<hgl/type/Stack.h>
#include<hgl/type/Map.h>

#include<iostream>

namespace hgl
{
    /**
    * 动态单调ID列表
    */
    template<typename T> 
    class DynamicMonoIDList
    {
        using ID=int32;

        ID id_count;

        DataArray<T>    item_list;
        
        Map<ID,int>     id_location_map;                    ///<ID位置映射表
        SortedSet<ID>   active_id_set;                      ///<活跃ID集合
        Stack<int>      free_location_set;                  ///<空闲位置集合

        ID AcquireID(){return ++id_count;}

    protected:
        
        
        int GetCount() const override
        {
            return item_list.GetCount();
        }

        void Expand(int size) override
        {
            item_list.Expand(size);
        }

        void *GetItemAt(int location) override
        {
            return item_list.At(location);
        }


    public:

        DynamicMonoIDList():id_count(0){}
        virtual ~DynamicMonoIDList() = default;

        int AddLocation()
        {
            ID new_id=AcquireID();

            if(free_location_set.IsEmpty())
            {
                int location=GetCount();
                Expand(1);
                id_location_map.Add(new_id,location);
                return location;
            }
            else
            {
                int location;
                free_location_set.Pop(location);
                id_location_map.Add(new_id,location);
                return location;
            }
        }

        bool Remove(ID id)
        {
            int location;
            
            if(!id_location_map.Get(id,location))
                return(false);

            if(location==-1)
                return(false);

            id_location_map.DeleteByKey(id);
            free_location_set.Push(location);

            return(true);
        }

        ID Add(const T &data)
        {
            int loc=AddLocation();

            if(loc<0)
                return(-1);

            memcpy(item_list.At(loc),&data,sizeof(T));
            return(id_count);
        }
    };//class DynamicMonoIDList
}//namespace hgl

using namespace hgl;
using namespace std;

int main(int,char **)
{
}
