#include<hgl/type/MonotonicIDList.h>
#include<hgl/type/SortedSet.h>

namespace hgl::ecs
{
    using EntityID=MonotonicID;
    constexpr const EntityID ENTITY_ID_INVALID=0;

    struct Entity
    {
        MonotonicID entity_id;          ///<当前实体的ID

        EntityID    child_id;           ///<子实体ID(<0为单个实体ID，>0为列表ID)
        EntityID    data_id;            ///<数据ID(<0为单个数据ID，>0为列表ID)

    public:

        void Init()
        {
            entity_id       =MONOTONIC_ID_INVALID;
            child_id        =ENTITY_ID_INVALID;
            data_id         =ENTITY_ID_INVALID;
        }

        bool IsValid() const{return entity_id>MONOTONIC_ID_INVALID;}

        bool hasChild           () const{return child_id!=ENTITY_ID_INVALID;}
        bool hasMultipleChild   () const{return child_id>ENTITY_ID_INVALID;}
        bool isSingleChild      () const{return child_id<ENTITY_ID_INVALID;}

        bool hasData        () const{return data_id!=ENTITY_ID_INVALID;}
        bool hasMultipleData() const{return data_id>ENTITY_ID_INVALID;}
        bool isSingleData   () const{return data_id<ENTITY_ID_INVALID;}
    };

    using EntityIDList=SortedSet<EntityID>;

    class EntityManager
    {
        MonotonicIDList<Entity,EntityID> entity_list;               ///<实体数据列表(使用单调ID列表，ID永不重复)
    };
}//namespace hgl::ecs