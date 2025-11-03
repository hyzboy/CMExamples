#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/Map.h>

#include<iostream>
#include<vector>

namespace hgl
{
    using MonotonicID = int32;

    /**
    * 单调ID列表(仅支持原生数据类型，不支持对象)
    */
    template<typename T,typename I = MonotonicID> class MonotonicIDList
    {
        struct DataPair
        {
            I id;
            T data;
        };

        I next_id = 0;

        DataArray<DataPair> data_array;
        Map<I,int32> id_to_location_map;
        Stack<int32> free_location;

    public:

        T *Add()
        {
            int location = -1;

            if(free_index.IsEmpty())
            {
                location = data_array.GetCount();

                data_array.Expand(1);
            }
            else
            {
                free_location.Pop(location);
            }

            DataPair *dp=data_array.At(location);
            
            dp->id=next_id;
            ++next_id;

            id_to_location_map.Add(dp->id,location);

            return &(dp->data);
        }

        bool Add(const T &value)
        {
            T *p=Add();

            if(!p)
                return(false);

            hgl_cpy<T>(*p,value);

            return(true);
        }

        bool Remove(const I &id)
        {
            int32 location;

            if(!id_to_location_map.Delete(id,location))     //Map::Delete的作用是获取这个数据并删除
                return(false);

            free_location.Push(location);

            return(true);
        }

        bool Contains(const I &id)const
        {
            return id_set.ContainsKey(id);
        }

        T *Get(const I &id)
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return(nullptr);

            return data_array.At(location);
        }
    };//template<typename T,typename I = MonotonicID> class MonotonicIDList
}//namespace hgl

using namespace hgl;
using namespace std;

int main(int,char **)
{
}
