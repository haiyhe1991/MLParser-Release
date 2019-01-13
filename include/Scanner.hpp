#pragma once
#include "Utility.hpp"
namespace Cyan
{
	enum TokenType
	{
		Default,
		LeftAngleBracket,
		EndTag,
		AttributeName,
		AttributeValue,
		RightAngleBracket
	};
	struct Token
	{
		TokenType type = Default;
		size_t offset = 0;
		string value = "";
		Token *next = nullptr;
		~Token()
		{
			//不可以递归删除，太多了会溢出
		}
	};
	class Scanner
	{
		//#ifdef _DEBUG
		//		FRIEND_TEST(Scanner_Test, GetAttributeValue_Test);
		//#endif
	private:
		const char *raw;
		Token *root;
		Token *now;
		void skipExcept(char exception, size_t &offset)
		{
			while (raw[offset] != exception && raw[offset] != '\0') ++offset;
		}
		void skipExcept(char exceptionA, char exceptionB, size_t &offset)
		{
			while (raw[offset] != exceptionA && raw[offset] != exceptionB && raw[offset] != '\0') ++offset;
		}
		void skipExcept(char exceptionA, char exceptionB, char exceptionC, size_t &offset)
		{
			while (raw[offset] != exceptionA && raw[offset] != exceptionB && raw[offset] != exceptionC && raw[offset] != '\0') ++offset;
		}
		void skipExcept(char exceptionA, char exceptionB, char exceptionC, char exceptionD, size_t &offset)
		{
			while (raw[offset] != exceptionA && raw[offset] != exceptionB && raw[offset] != exceptionC && raw[offset] != exceptionD && raw[offset] != '\0') ++offset;
		}
		void skipAll(char exception, size_t &offset)
		{
			while (raw[offset] == exception && raw[offset] != '\0') ++offset;
		}
		void skipAll(char exceptionA, char exceptionB, size_t &offset)
		{
			while ((raw[offset] == exceptionA || raw[offset] == exceptionB) && raw[offset] != '\0') ++offset;
		}
		void skipAll(char exceptionA, char exceptionB, char exceptionC, size_t &offset)
		{
			while ((raw[offset] == exceptionA || raw[offset] == exceptionB || raw[offset] == exceptionC) && raw[offset] != '\0') ++offset;
		}
		string gstrExcept(char exception, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exception, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
		string gstrExcept(char exceptionA, char exceptionB, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exceptionA, exceptionB, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
		string gstrExcept(char exceptionA, char exceptionB, char exceptionC, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exceptionA, exceptionB, exceptionC, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
		string gstrExcept(char exceptionA, char exceptionB, char exceptionC, char exceptionD, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exceptionA, exceptionB, exceptionC, exceptionD, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
	public:
		Scanner(const char *content) :raw(content), root(nullptr), now(nullptr)
		{
			root = new Token;
		}
		~Scanner()
		{ 
			Token *now = root, *then;
			while (now)
			{
				then = now->next;
				delete now;
				now = then;
			}
		}
		void Scan();
		Token *next()
		{
			now = now->next;
			return now;
		}

	};
}