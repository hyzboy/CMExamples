#include<hgl/type/ConstStringSet.h>
#include<hgl/type/Map.h>

#include<typeinfo>
#include<typeindex>
#include<iostream>

namespace hgl
{
    namespace IDNameManager
    {
        template<typename SC>
        bool RegistryIDName(ConstStringView<SC> &csv,const std::type_index &index,const SC *name_string)
        {
            static ObjectMap<std::type_index,ConstStringSet<SC>> css_set;

            ConstStringSet<SC> *css;

            if(css_set.KeyExist(index))
            {
                css_set.Get(index,css);
            }
            else
            {
                css=new ConstStringSet<SC>;
                css_set.Add(index,css);
            }

            return css->AddString(csv,name_string,hgl::strlen(name_string));
        }
    }

    /**
    * 顺序ID+名称数据结构模板<br>
    * 按添加进来的名字先后顺序一个个产生连续的序号，所有数据只可读不可写
    */
    template<typename SC,int CLASS_COUNTER> class OrderedIDName
    {
    protected:

        ConstStringView<SC> csv;

    protected:

        void Clear()
        {
            csv.id=-1;
            csv.length=-1;
            csv.str=nullptr;
        }

        void Update(const SC *name_string)
        {
            if(!name_string)
            {
                Clear();
            }
            else
            {
                IDNameManager::RegistryIDName<SC>(csv,typeid(*this),name_string);
            }
        }

    public:

        const int GetID     ()const{return csv.id;}                                                     ///<获取名称ID
        const SC *GetName   ()const{return csv.str;}                                                    ///<获取名称字符串

    public:

        OrderedIDName()
        {
            Clear();
        }

        OrderedIDName(const SC *name_string)
        {
            Update(name_string);
        }

        void operator = (const SC *name_string)
        {
            Update(name_string);
        }

        void operator = (const OrderedIDName<SC,CLASS_COUNTER> &id_name)
        {
            hgl_cpy(csv,id_name.csv);
        }

    public:

        const int Comp(const OrderedIDName &oin)const{return GetID()-oin.GetID();}

        CompOperator(const OrderedIDName &,Comp)
    };//class IDName

#define IDNameDefine(name,type) using name=OrderedIDName<type,__COUNTER__>; //使用__COUNTER__是为了让typeid()不同

    IDNameDefine(AnsiIDName,char)
    IDNameDefine(UTF8IDName,char)
}//namespace hgl

void main()
{
    using namespace hgl;

    UTF8IDName oin_utf8("oin_utf8");
    UTF8IDName oin_u8("oin u8");
    UTF8IDName oin_u8same(oin_u8);

    std::cout<<"oin_utf8:   "<<oin_utf8.GetID()<<std::endl;
    std::cout<<"oin_u8:     "<<oin_u8.GetID()<<std::endl;
    std::cout<<"oin_u8same: "<<oin_u8same.GetID()<<std::endl;
}