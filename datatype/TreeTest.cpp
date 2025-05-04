#include<iostream>
#include<string>

#include<hgl/type/TreeNode.h>

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
    ~FolderNode()=default;

    void OnAttachParent(TreeNode *node) override
    {
        std::cout<<(*node)->GetPath()<<"\\"<<(*this)->GetPath()<<std::endl;
    }

    void OnDetachParent() override
    {
        TreeNode *node=GetParent();

        std::cout<<"remove "<<(*this)->GetPath()<<" from "<<(*node)->GetPath()<<std::endl;
    }
};

int main(int,char **)
{
    using FolderManager=DataNodeManager<FolderNode>;

    FolderManager manager;

    FolderNode *root=manager.Create();

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

        delete linux;   //手动释放,这里不会产生FolderNode::OnDetachParent是正常现像。
                        //因为在完成FolderNode::~FolderNode后，虚拟函数的关系已回退到上一级，所以这一级的已经无效，所以无法产生调用。
                        //如果要实现调用，必须在FolderNode::~FolderNode调用DetachAll()

                        //这个问题我们可能需要调研，如要完全解决可能需要屏蔽delete操作符的使用，只允许使用Destory
    }

    //自动释放到可以了，但是释放顺序需要处理下
}
