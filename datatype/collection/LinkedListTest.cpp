template<typename KEY,typename VALUE>
    struct DataPair
    {
        KEY key;
        VALUE value;
    };

template<typename KEY,typename VALUE>
class UnorderedMap
{
    ObjectList<DataPair<KEY,VALUE>> data_list;
};