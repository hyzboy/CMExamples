#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/SmallMap.h>
#include<tsl/robin_set.h>
#include<tsl/robin_map.h>

#include<iostream>
#include<vector>

namespace hgl
{
    using MonotonicID = int32;

    /**
    * 单调ID数据列表(仅支持原生数据类型，不支持对象)
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
            if(free_location.IsEmpty())     //理论上free_location中没有数据，即便不需要收缩
                return 0;

            // 获取当前数据数组的大小
            int32 total_count = data_array.GetCount();
            int32 free_count = free_location.GetCount();
        
            // 如果所有位置都是空的，直接清空
            if(free_count >= total_count)
            {
                data_array.Clear();
                free_location.Clear();
                return free_count;
            }

            // 从后向前查找有效数据，填充到 free_location 中的空洞
            int32 moved_count = 0;
    
            while(!free_location.IsEmpty() && total_count > 0)
            {
                int32 free_loc;
                free_location.Pop(free_loc);
  
                // 如果空洞位置已经在末尾或之后，无需移动
                if(free_loc >= total_count - 1)
                {
                    --total_count;
                    continue;
                }
     
                // 从末尾找一个有效数据位置
                int32 last_valid_loc = total_count - 1;
                I last_valid_id = -1;
 
                // 查找末尾有效数据对应的ID
                bool found = false;
                for(auto it = id_to_location_map.Begin(); it != id_to_location_map.End(); ++it)
                {
                    if(it->value == last_valid_loc)
                    {
                        last_valid_id = it->key;
                        found = true;
                        break;
                    }
                }
       
                // 如果末尾也是空洞，继续向前查找
                if(!found)
                {
                    --total_count;
                    free_location.Push(free_loc); // 把当前空洞放回去，稍后处理
                    continue;
                }
            
                // 将末尾的有效数据移动到空洞位置
                T *src = data_array.At(last_valid_loc);
                T *dst = data_array.At(free_loc);
                hgl_cpy<T>(*dst, *src);
      
                // 更新 ID 到位置的映射
                id_to_location_map.Change(last_valid_id, free_loc);
      
                // 缩小数组大小
                --total_count;
                ++moved_count;
            }
  
            // 收缩 data_array 到实际使用的大小
            int32 original_count = data_array.GetCount();
            data_array.Resize(total_count);
   
            // 清空 free_location，因为所有空洞都已处理
            free_location.Clear();
            
            return original_count - total_count; // 返回收缩的数量
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

    cout<<"Shrinking the list...\n";
    list.Shrink();
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
