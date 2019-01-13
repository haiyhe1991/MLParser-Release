#include "Scanner.hpp"
namespace Cyan
{
		void Scanner::Scan()
	{
		size_t offset = 0;
		Token *lastToken = root;
		size_t len = strlen(raw);
		while (true)
		{
			if (offset >= len) break;
			if (raw[offset] == '<')
			{
				if (raw[offset + 1] == '/')
				{
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = EndTag;
					lastToken->offset = offset;
					offset += 2;//使得raw[offset]=tagName的第一位字符
					lastToken->value = gstrExcept('>', offset);
					str2lwr(lastToken->value);
					continue;
				}
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = LeftAngleBracket;
				lastToken->offset = offset;
				offset += 1;//使得raw[offset]=tagName的第一位字符
				lastToken->value = gstrExcept(' ', '/', '>', offset);
				str2lwr(lastToken->value);
				while (raw[offset] != '>')
				{
					skipAll(' ', offset);
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeName;
					lastToken->value = gstrExcept('=', ' ', '>', offset);
					skipAll(' ','=', offset);
					//skipAll('=', offset);

					//用单、双引号括起来的字符串里可能出现空格，为了正确分割
					//应该再再次遇到单、双引号的时候停止，而不是一刀切的在 空格、单双引号 处直接停止
					//也就是实现 将字符串视为一个整体来分割，即使字符串内包含空格（或单双引号）
					char flag = raw[offset];//当前字符是等号(=)右边的第一个非空格字符  name = ["]value"
					if (flag == '"' || flag == '\'') ++offset;//如果现在是引号，那么跳过，指向内容区
					//html code:
					///<text style="aaa" autofocus></text>
					//The attribute "autofocus" will cause a error
					//
					//the next judge is to fix this bug.
					if (flag == '>') break;
					
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeValue;
					if (flag == '"' || flag == '\'')
						lastToken->value = gstrExcept(flag, offset);
					else
						lastToken->value = gstrExcept(' ', '>', offset);
					skipAll('"', '\'', ' ', offset);
					skipAll('/', offset);
				}
			}
			if (raw[offset] == '>')
			{
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = RightAngleBracket;
				lastToken->offset = offset;
			}
			++offset;
		}
		now = root;
	}
}