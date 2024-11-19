#include<iostream>

static int BaseClassCount=0;

class BaseClass
{
    int count;

public:

    BaseClass()
    {
        count=BaseClassCount++;
        std::cout<<"BaseClass "<<count<<std::endl;
    }

    virtual ~BaseClass()
    {
        std::cout<<"~BaseClass "<<count<<std::endl;
    }
};

class SecondClassA:virtual public BaseClass
{
public:

    SecondClassA()
    {
        std::cout<<"SecondClassA"<<std::endl;
    }

    virtual ~SecondClassA()
    {
        std::cout<<"~SecondClassA"<<std::endl;
    }
};

class SecondClassB:virtual public BaseClass
{
public:

    SecondClassB()
    {
        std::cout<<"SecondClassB"<<std::endl;
    }

    virtual ~SecondClassB()
    {
        std::cout<<"~SecondClassB"<<std::endl;
    }
};

class ThirdClass:public SecondClassA,public SecondClassB
{
public:

    ThirdClass()
    {
        std::cout<<"ThirdClass"<<std::endl;
    }

    virtual ~ThirdClass()
    {
        std::cout<<"~ThirdClass"<<std::endl;
    }
};

class SpecialClass:public SecondClassA
{
public:

    SpecialClass()
    {
        std::cout<<"SpecialClass"<<std::endl;
    }

    virtual ~SpecialClass()
    {
        std::cout<<"~SpecialClass"<<std::endl;
    }
};

int main(int,char **)
{
    ThirdClass tc;

    BaseClass *bc=&tc;

    SecondClassA *a_from_tc=(SecondClassA *)&tc;
    SecondClassB *b_from_tc=(SecondClassB *)&tc;

    SecondClassA *a_cast_tc=dynamic_cast<SecondClassA *>(&tc);
    SecondClassB *b_cast_tc=dynamic_cast<SecondClassB *>(&tc);

    SecondClassA *a_cast_bc=dynamic_cast<SecondClassA *>(bc);
    SecondClassB *b_cast_bc=dynamic_cast<SecondClassB *>(bc);

    SpecialClass sc;
    BaseClass *sb=&sc;

    SecondClassA *a_from_sc=dynamic_cast<SecondClassA *>(&sc);
    SecondClassB *b_from_sc=dynamic_cast<SecondClassB *>(&sc);

    SecondClassA *a_from_sb=dynamic_cast<SecondClassA *>(sb);
    SecondClassB *b_from_sb=dynamic_cast<SecondClassB *>(sb);

    std::cout<<"tc: "<<&tc<<std::endl;

    std::cout<<"bc: "<<bc<<std::endl;

    std::cout<<"a_from_tc: "<<a_from_tc<<std::endl;
    std::cout<<"b_from_tc: "<<b_from_tc<<std::endl;

    std::cout<<"a_cast_tc: "<<a_cast_tc<<std::endl;
    std::cout<<"b_cast_tc: "<<b_cast_tc<<std::endl;

    std::cout<<"a_cast_bc: "<<a_cast_bc<<std::endl;
    std::cout<<"b_cast_bc: "<<b_cast_bc<<std::endl;

    std::cout<<"sc: "<<&sc<<std::endl;

    std::cout<<"a_from_sc: "<<a_from_sc<<std::endl;
    std::cout<<"b_from_sc: "<<b_from_sc<<std::endl;

    std::cout<<"a_from_sb: "<<a_from_sb<<std::endl;
    std::cout<<"b_from_sb: "<<b_from_sb<<std::endl;
    return 0;
}
