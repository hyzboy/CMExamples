#include<iostream>
#include<hgl/type/MemoryAllocator.h>
#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/List.h>

using namespace std;
using namespace hgl;

/**
 * 数据块管理器
 */
class DataBlockManager
{
    enum class ErrorCode
    {
        Success=0,                      ///<成功

        AcquireLengthError  =-1,        ///<申请长度错误
        FreeBlockNotEnough  =-2,        ///<没有足够的空间
        NotFitSegments      =-3,        ///<没有合适的数据块
    };

private:

    int block_bytes;                        ///<单个数据块字节数
    int block_count;                        ///<数据块数量
    uint64 total_bytes;                     ///<总字节数

    AbstractMemoryAllocator *allocator;      ///<内存分配器
    
private:

    /**
    * 用户数据
    */
    struct UserData
    {
        int bytes;                      ///<数据长度
        int block_count;                ///<占用数据块数量
        int block_start;                ///<起始数据块
    };//struct UserData

    /**
    * 数据块链表节点
    */
    struct BlockChainNode
    {
        BlockChainNode *prev;
        BlockChainNode *next;

        int start;                      ///<起始数据块
        int count;                      ///<数据块数量
    };//struct BlockChainNode

    BlockChainNode *bcn_data;           ///<数据块链表节点数据
    Stack<BlockChainNode *> bcn_stack;

    int free_block_count;

    BlockChainNode *bcn_start;
    BlockChainNode *bcn_end;


    List<UserData> user_data_list;

private:

    DataBlockManager(AbstractMemoryAllocator *aba,const int bs,const int bc)
    {
        allocator=aba;

        block_bytes=bs;
        block_count=bc;
        total_bytes=block_bytes*block_count;

        free_block_count=block_count;

        bcn_data=new BlockChainNode[block_count];
        {
            BlockChainNode *p=bcn_data;

            for(int i=0;i<block_count;i++)
            {
                bcn_stack.Push(p);
                ++p;
            }
        }

        bcn_stack.Pop(bcn_start);

        bcn_start->prev=nullptr;
        bcn_start->next=nullptr;
        bcn_start->start=0;
        bcn_start->count=block_count;

        bcn_end=bcn_start;
    }

protected:

    const int       GetBlockBytes   ()const{return block_bytes;}                ///<取得单个数据块字节数
    const int       GetBlockMaxCount()const{return block_count;}                ///<取得数据块最大数量
    const uint64    GetTotalBytes   ()const{return total_bytes;}                ///<取得总字节数

    const int       GetFreeBlockCount()const{return free_block_count;}          ///<取得剩余可用数据块数量总合(注：不代表直接申请这么大的块能成功)

public:

    virtual ~DataBlockManager()
    {
        delete[] bcn_data;
        SAFE_CLEAR(allocator);
    }

    /**
    * 请求数据块
    * @param acquire_bytes 请求的字节数
    * @param start_block 返回的起始数据块
    */
    ErrorCode Acquire(const int acquire_bytes,int *start_block)
    {
        if(acquire_bytes<=0)
            return ErrorCode::AcquireLengthError;

        UserData ud;

        ud.bytes=acquire_bytes;
        ud.block_count=(acquire_bytes+block_bytes-1)/block_bytes;

        if(ud.block_count>free_block_count)
            return ErrorCode::FreeBlockNotEnough;

        if(bcn_start==bcn_end)      //链接中只有一个节点，那就是空的了
        {
            free_block_count-=ud.block_count;

            bcn_start->start=ud.block_count;
            bcn_start->count=free_block_count;

            ud.block_start=0;

            user_data_list.Add(ud);

            *start_block=0;
            return ErrorCode::Success;
        }

        BlockChainNode *bcn=bcn_start;
        BlockChainNode *fit=nullptr;

        do
        {
            if(bcn->count==ud.block_count)      //正合适
            {
                fit=bcn;break;
            }

            if(bcn->count>ud.block_count)
            {
                if(!fit                         //没有合适的，先记下这个
                  ||fit->count>bcn->count)      //这个更小，更合适
                    fit=bcn;
            }

            bcn=bcn->next;
        }while(bcn!=bcn_end);

        if(!fit)    //没有合适的
            return ErrorCode::NotFitSegments;
        
        free_block_count-=ud.block_count;

        ud.block_start=fit->start;

        if(fit->count==ud.block_count)          //属于正好的，那方便，直接移除这个节点
        {
            fit->prev->next=fit->next;
            fit->next->prev=fit->prev;

            bcn_stack.Push(fit);                //收回这个链表节点
        }
        else
        {
            fit->start+=ud.block_count;
            fit->count-=ud.block_count;
        }
        
        user_data_list.Add(ud);

        *start_block=ud.block_start;
        return ErrorCode::Success;
    }

    ErrorCode Release()
    {
    }
};//class DataBlockManager

DataBlockManager *CreateDataBlockManager()
{
}
