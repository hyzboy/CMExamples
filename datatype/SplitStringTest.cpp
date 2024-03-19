#include<hgl/type/SplitString.h>
#include<hgl/type/MergeString.h>
#include<iostream>

using namespace hgl;
using namespace std;

void Output(UTF8StringList &sl,const char *str)
{
    cout<<str<<": ";

    for(int i=0;i<sl.GetCount();i++)
        cout<<"\""<<sl[i].c_str()<<"\" ";

    cout<<endl;
    sl.Clear();
}

int main(int,char **)
{
    UTF8StringList sl;

    UTF8String str="hello game world!";
    
    {
        UTF8String left,right;

        SpliteByString(str, UTF8String("game"), &left, &right);

        cout<<"SpliteByString"<<endl;
        cout << "left: \"" << left.c_str() <<"\"" << endl;
        cout << "right: \""<<right.c_str() <<"\"" << endl;
    }

    {
        SplitToStringListBySpace(sl,str);

        Output(sl,"SplitToStringListBySpace");
    }

    {
        SplitToStringListByChar(sl,str,' ');

        Output(sl,"SplitToStringListByChar");
    }

    {
        str="Hello\rgame,\nhello\r\nworld!";

        SplitToStringListByEnter(sl,str);
        
        Output(sl,"SplitToStringListByEnter");
    }

    {
        str="Hello, Game. World!";

        SplitToStringListByCodes(sl,str);
        
        Output(sl,"SplitToStringListByCodes");
    }

    {
        str="123,456 789-000";

        SplitToStringListByDigit(sl,str);
        
        Output(sl,"SplitToStringListByDigit");
    }

    {
        str="0x123,0x456 0x789-0x000";

        SplitToStringListByXDigit(sl,str);
        
        Output(sl,"SplitToStringListByXDigit");
    }

    {
        str="123.456 789 0.123 3.1415 .5";

        SplitToStringListByFloat(sl,str);
        
        Output(sl,"SplitToStringListByFloat");
    }

    {
        str="#include<hello.h>\n"
            "#include\"world.h\"\n";

        SplitToStringListByChars(sl,str,UTF8String("<>\"\n"));
        
        Output(sl,"SplitToStringListByChars");
    }
}
