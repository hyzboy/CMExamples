#include<hgl/log/log.h>

using namespace hgl;

struct TestObject
{
    OBJECT_LOGGER;

public:

    TestObject(int index)
    {
        // 设置实例名称为index的字符串形式
        Log.SetLoggerInstanceName(OSString::numberOf(index));

        LogVerbose("TestObject()");
    }

    ~TestObject()
    {
        LogVerbose("~TestObject()");
    }
};

int main()
{
    TestObject to(1);
    

    return 0;
}