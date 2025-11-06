#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/SmallMap.h>

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
        I next_id = 0;

        DataArray<T> data_array;
        SmallMap<I,int32> id_to_location_map;
        Stack<int32> free_location;

    public:

        T *Add()
        {
            int location = -1;

            if(free_location.IsEmpty())
            {
                location = data_array.GetCount();

                data_array.Expand(1);
            }
            else
            {
                free_location.Pop(location);
            }

            T *p = data_array.At(location);

            id_to_location_map.Add(next_id,location);

            ++next_id;

            return p;
        }

        bool Add(const T &value)
        {
            T *p = Add();

            if(!p)
                return(false);

            hgl_cpy<T>(*p,value);

            return(true);
        }

        bool Remove(const I &id)
        {
            int32 location;

            if(!id_to_location_map.GetAndDelete(id,location)) //Map::Delete的作用是获取这个数据并删除
                return(false);

            free_location.Push(location);

            return(true);
        }

        bool Contains(const I &id)const
        {
            return id_to_location_map.ContainsKey(id);
        }

        T *Get(const I &id)
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return(nullptr);

            return data_array.At(location);
        }

        int GetLocation(const I &id) const
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return -1;

            return location;
        }

        /**
        * 收缩数据存储空间
        */
        int Shrink()
        {
            if(free_location.IsEmpty())
                return 0;


        }
    };//template<typename T,typename I = MonotonicID> class MonotonicIDList
}//namespace hgl

using namespace hgl;
using namespace std;

static void PrintRange(MonotonicIDList<int> &list,int begin_id,int end_id)
{
    for(int id = begin_id; id <= end_id; ++id)
    {
        cout << "ID " << id << ": ";
        if(list.Contains(id))
        {
            int *v = list.Get(id);
            int location = list.GetLocation(id);
            if(v) cout << *v << " (location: " << location << ")";
            cout << '\n';
        }
        else
        {
            cout << "not found\n";
        }
    }
}

int main(int,char **)
{
    MonotonicIDList<int> list;

    // 批量添加10个数据，ID0..9
    for(int i = 0; i < 10; ++i)
        list.Add(i * 10);

    cout << "After initial insert (ID0..9):\n";
    PrintRange(list,0,9);

    // 删除多个ID
    const int to_remove[] = { 1,3,5,8 };
    for(int id : to_remove)
        list.Remove(id);

    cout << "After removing {1,3,5,8}:\n";
    PrintRange(list,0,9);

    // 再添加4个数据，ID10..13
    for(int i = 0; i < 4; ++i)
        list.Add(100 + i * 10);

    cout << "After inserting 4 more (ID10..13):\n";
    PrintRange(list,0,13);

    // 测试删除一个不存在的ID
    bool removed = list.Remove(42);
    cout << "Remove ID42 result: " << (removed ? "true" : "false") << '\n';

    return 0;
}
