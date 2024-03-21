#pragma once

#include<hgl/type/DataStackPool.h>
#include<hgl/type/SortedSets.h>

namespace hgl
{
    /**
     * 数据链管理器(注：它只管理这个链，并不管理数据)<br>
     * 它的思想是将空间分配成一个个固定长度的单位，然后通过链表来管理这些单位的使用情况。
     * 当用户释放一个空间时，会自动合并相邻的空间节点。如果没有相邻的空间节点，会自动创建一个新的空间节点。
     * 当用户申请一个新的空间时，会自动查找最适合的空间节点。
     */
    class DataChain
    {
        SeriesInt series;                   ///<序号池

        int max_count;                      ///<最大数量
    
    protected:

        /**
        * 用户数据占用信息
        */
        struct UserData
        {
        private:

            int start;                      ///<起始数据块
            int count;                      ///<占用数据块数量

            friend class DataChain;

        public:
            
            const int GetStart()const{return start;}
            const int GetCount()const{return count;}
            const int GetEnd()const{return start+count;}
        };//struct UserData

        DataStackPool<UserData> ud_pool;    ///<用户数据占用信息池

        SortedSets<UserData *> ud_set;      ///<用户数据占用合集

    protected:

        /**
        * 数据块链表节点
        */
        struct ChainNode
        {
            ChainNode *prev;
            ChainNode *next;

            int start;                      ///<起始数据块
            int count;                      ///<数据块数量

        public:
            
            const int GetEnd()const{return start+count;}
        };//struct ChainNode

        DataStackPool<ChainNode> node_pool; ///<链表节点池

        int free_count;                     ///<空闲数据块数量
        
        ChainNode *start,*end;

    public:

        const int GetMaxCount   ()const{return max_count;}
        const int GetFreeCount  ()const{return free_count;}

    public:

        DataChain(const int mc):series(mc),node_pool(mc),ud_pool(mc)
        {
            max_count=mc;
            free_count=mc;

            ud_set.PreAlloc(mc);

            start=node_pool.Acquire();
            end=start;

            start->prev=nullptr;
            start->next=nullptr;
            start->start=0;
            start->count=max_count;
        }

        virtual ~DataChain()=default;

        UserData *Acquire(const int acquire_count)
        {
            if(acquire_count<=0)
                return(nullptr);

            if(acquire_count>free_count)
                return(nullptr);

            if(acquire_count>max_count)
                return(nullptr);

            if(start==end)
            {
                free_count-=acquire_count;

                start->start=acquire_count;
                start->count=free_count;

                UserData *ud=ud_pool.Acquire();

                ud->start=0;
                ud->count=acquire_count;

                ud_set.Add(ud);

                return(ud);
            }

            ChainNode *cn=start;
            ChainNode *fit=nullptr;

            do
            {
                if(cn->count==acquire_count)    //长度正合适，那就用这一段
                {
                    fit=cn;
                    break;
                }
            
                if(cn->count>acquire_count)     //长度够了
                {
                    if(!fit                     //没有合适的，先记下这个
                      ||fit->count>cn->count)   //这个更小，更合适
                        fit=cn;
                }
            
                cn=cn->next;
            }while(cn!=end);

            if(!fit)    //没有合适的
                return(nullptr);

            free_count-=acquire_count;
            
            UserData *ud=ud_pool.Acquire();

            ud->start=fit->start;
            ud->count=acquire_count;

            ud_set.Add(ud);

            if(fit->count==acquire_count)       //正好合适
            {
                if(fit->prev)
                    fit->prev->next=fit->next;
                else
                    start=fit->next;
            
                if(fit->next)
                    fit->next->prev=fit->prev;
                else
                    end=fit->prev;
            
                node_pool.Release(fit);
            }
            else
            {
                fit->start+=acquire_count;
                fit->count-=acquire_count;
            }

            return(ud);
        }

        bool Release(UserData *ud)
        {
            if(!ud)
                return(nullptr);

            if(!ud_set.IsMember(ud))
                return(false);

            const int ud_end=ud->GetEnd();

            ChainNode *cur=start;
            ChainNode *next;
            int cur_end;

            do
            {
                cur_end=cur->GetEnd();

                if(cur_end==ud->start)          //接在一起了
                {
                    cur->count+=ud->count;      //count扩充
               
                    return(true);
                }

                next=cur->next;

                if(next)
                {
                    if(next->start==ud_end)     //下一个块接着，那就合并
                    {
                        next->count+=ud->count;    //count扩充
                        next->start-=ud->count;    //start前移,理论上==ud->start，调试时检查一下

                        return(true);
                    }
                    else
                    if(next->start>ud_end)      //下一个块不接着，但是在后面，那就新建一个节点
                    {
                        ChainNode *new_cn=node_pool.Acquire();
                                       
                        new_cn->prev=cur;
                        new_cn->next=next;
                        new_cn->start=ud->start;
                        new_cn->count=ud->count;

                        cur->next=new_cn;
                        next->prev=new_cn;
                    
                        return(true);
                    }
                    else //next->start<ud_end，，，这应该是一个bug
                    {
                        return(false);
                    }
                }
                else    //都没下一个了，那这个就当下一个吧
                {
                    ChainNode *new_cn=node_pool.Acquire();
                    
                    new_cn->prev=cur;
                    new_cn->next=nullptr;
                    new_cn->start=ud->start;
                    new_cn->count=ud->count;

                    cur->next=new_cn;
                    end=new_cn;
                    
                    return(true);
                }

                cur=next;
            }while(cur<=end);
        }
    };//class DataChain
}//namespace hgl
