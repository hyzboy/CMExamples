#include<hgl/type/DataChain.h>
#include<iostream>
#include<random>

using namespace std;
using namespace hgl;

void out_data_chain(DataChain *dc)
{
    DataChain::ChainNode *start =dc->GetStartNode();
    DataChain::ChainNode *end   =dc->GetEndNode();

    DataChain::ChainNode *node  =start;

    cout<<"Data Chain: ";

    while(node)
    {
        cout<<"["<<node->start<<","<<node->count<<"]";

        if(node==end)
            break;

        node=node->next;
    }

    cout<<endl;
}

int os_main(int,os_char **)
{
    DataChain dc(100);         ///数据链管理器(预定100个块)

    cout<<"DataChain Test"<<endl;

    DataChain::UserNode *un[10];

    for(int i=0;i<10;i++)
    {
        un[i]=dc.Acquire(10);

        if(!un[i])
        {
            cout<<"Acquire UserNode Error!"<<endl;
            break;
        }
        else
        {
            cout<<"Acquire UserNode: "<<un[i]->GetStart()<<","<<un[i]->GetCount()<<endl;
        }

        out_data_chain(&dc);
    }

    cout<<"---------------------------------------------------------------------------------"<<endl;
    cout<<"Release 0"<<endl;
    dc.Release(un[0]);
    out_data_chain(&dc);

    cout<<"Release 5"<<endl;
    dc.Release(un[5]);
    out_data_chain(&dc);

    cout<<"Release 9"<<endl;
    dc.Release(un[9]);
    out_data_chain(&dc);

    cout<<"Release 6"<<endl;
    dc.Release(un[6]);
    out_data_chain(&dc);

    cout<<"---------------------------------------------------------------------------------"<<endl;
    cout<<"Acquire 20"<<endl;
    DataChain::UserNode *ud20=dc.Acquire(20);
    out_data_chain(&dc);

    cout<<"Release 2"<<endl;
    dc.Release(un[2]);
    out_data_chain(&dc);
    cout<<"Release 3"<<endl;
    dc.Release(un[3]);
    out_data_chain(&dc);

    cout<<"Acquire 15"<<endl;
    DataChain::UserNode *ud15=dc.Acquire(15);
    out_data_chain(&dc);
}