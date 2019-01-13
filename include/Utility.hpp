#pragma once
#include <iostream>
#include <string>
#include <exception>
using std::string;
//这里保存一些有效但是不安全的代码
//使用这些函数需要小心
namespace Cyan
{
	inline char *strcpy(const char *src)
	{
		size_t len = strlen(src) + 1;
		char *t = new char[len];
		memcpy(t, src, len);
		return t;
	}
	inline string substr(const char *src, size_t count)
	{
		char *t = new char[count + 1]();
		memcpy(t, src, count);
		string s = string(t);
		delete[] t;
		return s;
	}
	inline void str2lwr(string &m)
	{
		for (size_t i = 0; i < m.size(); i++)
		{
			m[i] = (char)tolower(m[i]);
		}
	}
}