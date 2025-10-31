#include "DynamicMonoIDLocator.h"

namespace hgl
{
 int DynamicMonoIDLocator::BuildCompactMapping(IDUpdateList &mapping) const
 {
 int src_count = GetMapCount();

 mapping.Resize(src_count);

 if(src_count<=0)
 return 0;

 IDUpdateItem *ip = mapping.GetData();
 auto id_loc = id_location_map.GetDataList();

 MonotonicID new_id =0;

 for(int i=0;i<src_count;i++)
 {
 ip->old_id = (*id_loc)->key;
 ip->new_id = new_id;

 ++ip;
 ++new_id;
 ++id_loc;
 }

 return new_id;
 }

 int DynamicMonoIDLocator::CompactLocations(LocationUpdateList *mapping)
 {
 int src_count = GetMapCount();
 if(src_count<=0)
 {
 // 清空 free set
 free_location_set.Clear();
 if(mapping) mapping->Resize(0);
 return 0;
 }

 //先收集所有 entries并找出最大位置
 auto id_loc = id_location_map.GetDataList();

 int max_loc = -1;
 // count non-null entries to determine real_src_count
 int real_count =0;
 for(int i=0;i<src_count;i++)
 {
 auto kv = id_loc[i];
 if(!kv) continue;
 ++real_count;
 if(kv->value > max_loc) max_loc = kv->value;
 }

 if(real_count==0)
 {
 free_location_set.Clear();
 if(mapping) mapping->Resize(0);
 return 0;
 }

 // Prepare mapping array
 if(mapping) mapping->Resize(real_count);
 LocationUpdateItem *mp = mapping ? mapping->GetData() : nullptr;

 // build location -> index (into mapping) and mapping entries following current map order
 std::vector<int> loc_to_index(max_loc+1, -1);
 std::vector<int> mapping_index_by_order; mapping_index_by_order.reserve(real_count);

 int map_idx =0;
 for(int i=0;i<src_count;i++)
 {
 auto kv = id_loc[i];
 if(!kv) continue;
 MonotonicID id = kv->key;
 int old_loc = kv->value;

 if(mp)
 {
 mp[map_idx].old_id = id;
 mp[map_idx].old_location = old_loc;
 mp[map_idx].new_location = -1;
 }

 loc_to_index[old_loc] = map_idx;
 mapping_index_by_order.push_back(map_idx);
 ++map_idx;
 }

 // find holes (unused positions) in [0..max_loc]
 std::vector<int> holes;
 holes.reserve(max_loc+1);
 for(int i=0;i<=max_loc;i++)
 {
 if(loc_to_index[i]==-1) holes.push_back(i);
 }

 // now move items from end into holes to minimize moves
 int end = max_loc;
 // find initial end which has an item
 while(end>=0 && (end >= (int)loc_to_index.size() || loc_to_index[end]==-1)) --end;

 // new map to build
 Map<MonotonicID,int> new_map;
 SortedSet<MonotonicID> new_active;

 // iterate holes from smallest to largest, fill each with an item from the end
 for(int h_idx=0; h_idx<(int)holes.size() && end>holes[h_idx]; ++h_idx)
 {
 int hole = holes[h_idx];

 // find next end that has an item
 while(end>hole && (end >= (int)loc_to_index.size() || loc_to_index[end]==-1)) --end;
 if(end<=hole) break;

 int idx = loc_to_index[end];
 if(idx==-1) { --end; --h_idx; continue; }

 // move item at 'end' to 'hole'
 MonotonicID id = mp ? mp[idx].old_id : -1;

 if(mp) mp[idx].new_location = hole;
 new_map.Add(id, hole);
 new_active.Add(id);

 // update mapping index table
 loc_to_index[hole] = idx;
 loc_to_index[end] = -1;

 --end;
 }

 // remaining items (including those already placed) should occupy increasing positions starting at0..max
 int write_pos =0;
 for(int pos=0; pos<=max_loc; ++pos)
 {
 int idx = (pos < (int)loc_to_index.size()) ? loc_to_index[pos] : -1;
 if(idx==-1) continue;

 MonotonicID id = mp ? mp[idx].old_id : -1;
 if(mp && mp[idx].new_location==-1) mp[idx].new_location = write_pos;
 new_map.Add(id, write_pos);
 new_active.Add(id);
 ++write_pos;
 }

 // sizes
 int new_count = write_pos;

 id_location_map = std::move(new_map);
 active_id_set = std::move(new_active);

 // 清空 free locations
 free_location_set.Clear();

 return new_count;
 }

 void DynamicMonoIDLocator::Reset(int new_id_count)
 {
 id_count = (new_id_count>0)?(new_id_count-1):-1;
 free_location_set.Clear();

 id_location_map.Clear();
 active_id_set.Resize(new_id_count);

 for(int i=0;i<new_id_count;i++)
 {
 id_location_map.Add(i,i);
 active_id_set.Add(i);
 }
 }
}
