# 简介
MLParser（Markup Language Parser）是支持解析HTML、XML的解析器。有较高的容错性、对中文支持良好。完备的接口，能轻松提取HTML之中的内容。
# 使用方法
### 1.快速入门（获取内容）
```C++
#include <iostream>
#include <string>
#include <MLParser>
using namespace std;
using namespace Cyan;
int main()
{
    string hStr("<html><head><title>Hello MLParser</title></head></html>");
    MLParser ml;
    ml.Parse(hStr);
    ml["html"]["head"]["title"];
    if(ml.OK())//请不要省略这一判断
    {
        cout<<ml.GetInner()<<endl;
    }
    else
    {
        cout<<ml.GetErrorMsg()<<endl;
        ml.PrintTree();//这个函数会打印解析产生的树结构，用来检查解析是否正确
    }
    ml.Dispose();//千万不要忘了这一步
    return 0;
}
```
以上代码输出：
>Hello MLParser
---------
### 2.获取属性、获取（去除了HTML标签的）内容
```C++
#include <iostream>
#include <string>
#include <MLParser>
using namespace std;
using namespace Cyan;
int main()
{
    string hStr("<html><body><a herf="http://github.com/">Click Here!<img></img></a></body></html>");
    MLParser ml;
    ml.Parse(hStr);
    ml["html"]["body"]["a"];
    if(ml.OK())//请不要省略这一判断
    {
        string attributeValue;
        bool Founded = ml.FindAttribute("herf",attributeValue);
        if(Founded)
        {
            cout<<attributeValue<<endl;
        }
        else
        {
            cout<<ml.GetErrorMsg()<<endl;
        }
        //以上是获取属性，下面是获取内容
        cout<<ml.GetInner()<<endl;
        cout<<ml.GetContent()<<endl;//这样获取的内容会把HTML标签去除
    }
    else
    {
        cout<<ml.GetErrorMsg()<<endl;
        ml.PrintTree();//这个函数会打印解析产生的树结构，用来检查解析是否正确
    }
    ml.Dispose();//千万不要忘了这一步
    return 0;
}
```
以上代码输出：
>http://github.com/

>Click Here!`<img></img>`

>Click Here!
---------
### 3.搜索
```C++
#include <iostream>
#include <string>
#include <MLParser>
using namespace std;
using namespace Cyan;
int main()
{
    string hStr("<html><body><ul><li>aaa</li><li>bbb</li></ul></body></html>");
    MLParser ml;
    ml.Parse(hStr);
    Results rs = ml.SearchByTagName("li");
    for(auto result : rs)
    {
        cout<<result.GetInner()<<endl;
    }
    ml.Dispose();//千万不要忘了这一步
    return 0;
}
```
以上代码输出：
>aaa

>bbb