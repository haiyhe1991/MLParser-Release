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
    try
    {
        cout << ml["html"]["head"]["title"].GetInner() << endl;//也可以使用GetContent获得去除html标签后的内容
    }
    catch (CantFindTag &cft)
    {
        cout << cft.what() << endl;
        //使用 ml.PrintTree() 打印树结构 检查是否解析正确
    }
    catch (CantFindAttribute &cfa)
    {
        cout << cfa.what() << endl;
    }
    return 0;
}
```
以上代码输出：
> * Hello MLParser
---------
### 2.获取属性
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
    try
    {
        cout << ml["html"]["body"]["a"].GetAttribute("herf") << endl;
    }
    catch (CantFindTag &cft)
    {
        cout << cft.what() << endl;
        //使用 ml.PrintTree() 打印树结构 检查是否解析正确
    }
    catch (CantFindAttribute &cfa)
    {
        cout << cfa.what() << endl;
    }
    return 0;
}
```
以上代码输出：
> * http://github.com/
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
> * aaa
> * bbb