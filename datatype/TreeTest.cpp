#include<iostream>
#include<string>

#include<typeinfo>
#include<hgl/TypeFunc.h>
#include<tsl/robin_map.h>
#include<tsl/robin_set.h>


namespace hgl
{
    class IDNodeManager;

    template<typename T> class TreeNodeManager;

    enum class NodeLifePhase
    {
        None=0,                 ///<未知
        Work,                   ///<工作中(一般的使用状态)
        Recycled,               ///<已回收(走过了回收流程，但并未释放资源)
        WaitDestory,            ///<等待销毁(已经进入真正的销毁流程)
        Destory,                ///<销毁(已经被销毁，不存这种对像，只在根据ID获取状态时可以得到)

        ENUM_CLASS_RANGE(None,Destory)
    };

    class IDNode
    {
        IDNodeManager *manager;         ///<管理器指针
        size_t unique_id;               ///<唯一ID

        IDNode *parent_node;                            ///<父节点指针
        tsl::robin_map<size_t,IDNode *> child_map;      ///<子节点集合

        NodeLifePhase life_phase;       ///<生命周期状态
        
    public:

                IDNodeManager *GetManager()     {return manager;}                                   ///<获取管理器指针
        const   IDNodeManager *GetManager()const{return manager;}                                   ///<获取管理器指针

        const size_t GetManagerID   ()const;                                                        ///<获取管理器ID
        const size_t GetUniqueID    ()const{return unique_id;}                                      ///<获取对象唯一ID

        const NodeLifePhase GetLifePhase()const{return life_phase;}                                 ///<获取生命周期状态

        const bool IsWork       ()const{return life_phase==NodeLifePhase::Work;}                    ///<可以工作
        const bool IsRecycled   ()const{return life_phase==NodeLifePhase::Recycled;}                ///<是否已经回收
        const bool IsWaitDestory()const{return life_phase==NodeLifePhase::WaitDestory;}             ///<是否已经进入等待销毁状态

    private:

        friend class IDNodeManager;
        template<typename T> friend class TreeNode;
        template<typename T> friend class TreeNodeManager;

        IDNode(IDNodeManager *nm,const size_t uid)
        {
            manager=nm;
            unique_id=uid;

            life_phase=NodeLifePhase::None;

            parent_node=nullptr;
        }

        virtual ~IDNode();

        virtual void MarkWaitDestory()
        {
            life_phase=NodeLifePhase::WaitDestory;
        }

    public:
        
        virtual void OnDestory()
        {
            life_phase=NodeLifePhase::Destory;        //设置为销毁状态
            delete this;
        }

        virtual void Destory();

    public: //子节点相关

        virtual void OnAttachParent(IDNode *pn){parent_node=pn;}                                    ///<被附加到父节点时调用(参见AttachChild)
        virtual void OnDetachParent(IDNode *pn){parent_node=nullptr;}                               ///<被从父节点中移除时调用(参见DetachChild)
        const IDNode *GetParent()const{return parent_node;}                                         ///<获取父节点指针

        const size_t GetChildCount()const{return child_map.size();}                                 ///<获取子节点数量

        const bool Contains(const size_t id)const{return child_map.contains(id);}                   ///<根据ID判断是否包含子节点
        const bool Contains(const IDNode *node)const
        {
            if(!node)return(false);
            if(node->GetManager()!=GetManager())return(false);

            return Contains(node->GetUniqueID());
        }

        const bool AttachChild(IDNode *node)
        {
            if(!node)return(false);
            if(Contains(node))return(false);

            child_map.emplace(node->GetUniqueID(),node);
            node->OnAttachParent(this);
            return(true);
        }

        void DetachChild(IDNode *node)
        {
            if(!node)return;

            if(node->GetManager()!=GetManager())
                return;

            const size_t child_id=node->GetUniqueID();

            if(child_map.contains(child_id))
                child_map.erase(child_id);

            node->OnDetachParent(this);
        }

        void DestoryAllChild()
        {
            for(auto &it:child_map)
            {
                IDNode *node=it.second;

                if(!node)
                    continue;
                
                child_map.erase(node->GetUniqueID());   //从子节点集合中移除
                node->OnDetachParent(this);             //从父节点中移除

                node->Destory();                        //销毁子节点
            }

            child_map.clear();
        }
    };//class IDNode

    class IDNodeManager
    {
        static const size_t AcquireManagerID()
        {
            static size_t id_count=0;

            return ++id_count;
        }

    private:

        size_t manager_serial;
        size_t node_serial;
        
    private:

        tsl::robin_map<size_t,IDNode *> node_map;           ///<节点集合

        tsl::robin_set<IDNode *> wait_destory_node_set;     ///<等待销毁的节点集合

        tsl::robin_set<IDNode *> destored_node_set;         ///<已经销毁的节点集合

    protected:

        friend class IDNode;

        const size_t    AcquireNodeID(){return ++node_serial;}

        virtual IDNode *OnCreateNode(const size_t node_id)=0;                   ///<创建节点时调用
        virtual void    OnDestoryNode(IDNode *node)=0;                          ///<销毁节点时调用

        void            OnNodeDirectDestory(IDNode *node)                       ///<直接销毁，这种情况只在对象被直接delete的情况下，一般不需要
        {
            if(!node)return;

            if(wait_destory_node_set.contains(node))
                wait_destory_node_set.erase(node);

            if(!destored_node_set.contains(node))
                destored_node_set.emplace(node);
        }

    public:

        const size_t GetMangaerID()const{return manager_serial;}

    public:

        IDNodeManager()
        {
            manager_serial=AcquireManagerID();
            node_serial=0;
        }

        virtual ~IDNodeManager(){}

        virtual void ForceClear()
        {
            if(!node_map.empty())
            {
                for(auto &it:node_map)
                    OnDestoryNode(it.second);   //销毁节点

                //这里的逻辑会有问题，需要更简单的逻辑

                node_map.clear();
            }

            if(!wait_destory_node_set.empty())
            {
                for(auto *node:wait_destory_node_set)
                    OnDestoryNode(node);           //销毁节点

                wait_destory_node_set.clear();
            }
        }

        IDNode *CreateNode()
        {
            const size_t node_id=AcquireNodeID();

            IDNode *node=OnCreateNode(node_id);

            if(!node)
                return(nullptr);

            node_map.emplace(node_id,node);

            return(node);
        }

        const bool ContainsNode(IDNode *tn)const
        {
            if(!tn)return(false);
            if(tn->GetManagerID()!=GetMangaerID())return(false);

            if(!node_map.contains(tn->GetUniqueID()))
                return(false);

            return(true);
        }

        bool DestoryNode(IDNode *node)
        {
            if(!node)return(false);

            if(wait_destory_node_set.contains(node))
                return(true);
            
            if(!ContainsNode(node))
                return(false);

            node->MarkWaitDestory();
            node_map.erase(node->GetUniqueID());
            wait_destory_node_set.insert(node);

            return(true);
        }

        IDNode *GetNode(const size_t node_id)
        {
            auto iter=node_map.find(node_id);

            if(iter==node_map.end())
                return(nullptr);

            return(iter->second);
        }

        virtual void Update()
        {

        }
    };//class IDNodeManager

    const size_t IDNode::GetManagerID()const
    {
        return manager->GetMangaerID();
    }

    void IDNode::Destory()
    {
        manager->DestoryNode(this);
    }

    IDNode::~IDNode()
    {
        if(GetChildCount())
            DestoryAllChild();

        if(parent_node)
            parent_node->DetachChild(this);             //从父节点中移除

        if(life_phase<NodeLifePhase::WaitDestory)       //还没有进入销毁器
            manager->OnNodeDirectDestory(this);         //直接销毁
    }

    template<typename T> class TreeNode:public IDNode
    {
        T node_data;

    private:

        friend class TreeNodeManager<T>;        ///<树节点管理器

        using IDNode::IDNode;

    public:

        virtual ~TreeNode()override{}

    public:

        operator        T &()       {return node_data;}
        operator const  T &()const  {return node_data;}

                T &operator *()     {return node_data;}
        const   T &operator *()const{return node_data;}

                T &get()       {return node_data;}
        const   T &get()const  {return node_data;}

                T *operator ->()        {return &node_data;}
        const   T *operator ->()const   {return &node_data;}

        T &operator = (const T &data)
        {
            node_data=data;
            return node_data;
        }
    };//class TreeNode

    template<typename T> class TreeNodeManager:public IDNodeManager
    {
    public:

        using NodeType=TreeNode<T>;                         ///<节点类型
        
    protected:

        IDNode *OnCreateNode(const size_t node_id) override
        {
            return(new NodeType(this,node_id));
        }

        void OnDestoryNode(IDNode *node)override
        {
            if(!node)return;

            node->OnDestory();
        }

    public:

        using IDNodeManager::IDNodeManager;
        virtual ~TreeNodeManager()
        {
            ForceClear();
        }

        NodeType *Create(){return (NodeType *)CreateNode();}
    };//class TreeNodeManager
}//namespace hgl

using namespace hgl;

class Folder
{
    std::string path;

public:

    Folder()
    {
        path.clear();
    }

    ~Folder()
    {
        std::cout<<"~Folder(\""<<path<<"\")"<<std::endl;
    }

    void SetPath(const std::string &p)
    {
        path=p;

        std::cout<<"Folder:\""<<path<<"\""<<std::endl;
    }

    const std::string &GetPath()const
    {
        return path;
    }
};

class FolderNode:public TreeNode<Folder>
{
public:

    using TreeNode<Folder>::TreeNode;

    void OnAttachParent(IDNode *node) override
    {
        TreeNode<Folder>::OnAttachParent(node);

        TreeNode<Folder> *pn=(TreeNode<Folder> *)node;

        std::cout<<(*pn)->GetPath()<<"\\"<<(*this)->GetPath()<<std::endl;
    }

    void OnDetachParent(IDNode *node) override
    {
        TreeNode<Folder>::OnDetachParent(node);

        TreeNode<Folder> *pn=(TreeNode<Folder> *)node;

        std::cout<<"remove "<<(*this)->GetPath()<<" from "<<(*pn)->GetPath()<<std::endl;
    }
};

int main(int,char **)
{
    using FolderManager=TreeNodeManager<Folder>;

    FolderManager manager;

    auto *root=manager.Create();

    (*root)->SetPath("root");

    {
        auto *win=manager.Create();
        (*win)->SetPath("win");
        root->AttachChild(win);
    }

    {
        auto *linux=manager.Create();
        (*linux)->SetPath("linux");
        root->AttachChild(linux);
    }

    //自动释放到可以了，但是释放顺序需要处理下
}