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

    void OnAttachParent(TreeBaseNode *node) override
    {
        TreeNode<Folder>::OnAttachParent(node);

        TreeNode<Folder> *pn=(TreeNode<Folder> *)node;

        std::cout<<(*pn)->GetPath()<<"\\"<<(*this)->GetPath()<<std::endl;
    }

    void OnDetachParent(TreeBaseNode *node) override
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
