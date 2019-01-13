#include "MLParser.h"
#include "Scanner.hpp"
#include <stack>
#include <regex>
#include <list>
using std::list;
using std::stack;
using std::string;
namespace Cyan
{
	void MLParser::preprocess(string &m)
	{
		//以下代码用于去除script标签、style标签与注释
		std::regex exp1("<script([\\S\\s]+?)<\\/script>");
		m = std::regex_replace(m, exp1, "");
		std::regex exp2("<!--([\\S\\s]+?)-->");
		m = std::regex_replace(m, exp2, "");
		std::regex exp3("<!([\\S\\s]+?)>");
		m = std::regex_replace(m, exp3, "");
		std::regex exp4("<style([\\S\\s]+?)<\\/style>");
		m = std::regex_replace(m, exp4, "");

		raw = Cyan::strcpy(m.data());
		//以下代码用于去除\n \r \t
		size_t i, k;
		for (i = k = 0; raw[i] != '\0'; ++i)
		{
			if (raw[i] == '\n' || raw[i] == '\r' || raw[i] == '\t') {}
			else
			{
				raw[k] = raw[i];
				k = k + 1;
			}
		}
		raw[k] = '\0';
	}
	bool MLParser::Parse(string html)
	{
		preprocess(html);
		//initial Scanner & Scan
		Scanner SC = Scanner(raw);
		SC.Scan();

		stack<Node *> NodeStack;
		list<Node *> NodeList;
		root = new Node();
		root->tagName = "_root_";
		Node *lNode = root;//记录上一个操作过的Node
		Node *pNode = root;//下一个新节点的父亲节点
		Node *tNode = root;//便于更新Node信息
		NodeStack.push(root);
		Attribute *lAttribute = nullptr;//记录上一个操作过的Attribute;

		//****************************************
		//因为设计错误，不得不使用该变量记录一些信息
		//遇到LeftAngleBracket和EndTag会改变这个值（改为L_A_B或者EndTag）
		//遇到RightAngleBracket会使用这个值：
		//如果该变量(tType) == LeftAngleBracket
		//那么RightAngleBracket就修改lNode的txtOffset字段为它自身的offset
		//反之则不做任何操作
		//****************************************
		TokenType tType = Default;

		Token *token = nullptr;
		while ((token = SC.next()) != nullptr)
		{
			switch (token->type)
			{
			case LeftAngleBracket:
				tNode = pNode->CreateChild();//Create a new child node
				tNode->tagName = token->value;
				pNode = lNode = tNode;
				AddTag(tNode);//用于模糊搜索
				NodeStack.push(tNode);
				NodeList.push_back(tNode);
				tType = LeftAngleBracket;//见tType声明前注释
				break;
			case AttributeName:
				lAttribute = lNode->AddAttribute(token->value, "");
				AddAttribute(token->value, lNode);//用于模糊搜索
				break;
			case AttributeValue:
				lAttribute->value = token->value;
				break;
			case EndTag:
				if (NodeStack.size() <= 1)break;
				tNode = NodeStack.top();
				NodeStack.pop();
				NodeList.pop_back();
				while (tNode->tagName != token->value)
				{
					list<Node *>::iterator it;
					for (it = NodeList.begin(); it != NodeList.end(); ++it)
					{
						if ((*it)->tagName == token->value) break;
					}
					if (it == NodeList.end())
					{
						//如果说tNode->tagName != token->value，就不会出现it == NodeList.end()
						//说明这是凭空出现的一个End_Tag
						//这个凭空出现的End_Tag不应该造成任何影响
						//所以我们把弹出来的Node再加回去
						NodeStack.push(tNode);
						NodeList.push_back(tNode);
						break;
					}
					//下面的代码用于修复parent的指向错误
					if(tNode->child != nullptr)
						tNode->child->MoveTo(tNode->parent);
					if (NodeStack.size() <= 1) 
						break;
					tNode = NodeStack.top();
					NodeStack.pop();
					NodeList.pop_back();
				}
				tNode->count = token->offset - tNode->txtOffset;
				pNode = NodeStack.top();
				tType = EndTag;//见tType声明前注释
				break;
			case RightAngleBracket:
				if (tType == LeftAngleBracket)//见tType变量声明的注释
				{
					lNode->txtOffset = token->offset + 1;//token->offset指向右尖括号，+1改为指向内容的第一个字符
				}
				break;
			default: 
				break;
			}
		}
		now = root;
		return true;
	}
	void MLParser::PrintTree(bool printAttributes)
	{
		Print(now, 0, printAttributes, false);
	}
	void MLParser::Print(Cyan::Node *node, size_t count,bool printAttribute,bool printBrother)
	{
		using std::cout;
		using std::endl;
		if (node != nullptr)
		{
			for (size_t i = 0; i < count; ++i)
			{
				cout << "·";
			}
			cout << '<' << node->tagName;
			if (printAttribute)
			{
				Attribute *tA = node->attributes;
				while (tA != nullptr)
				{
					cout << ' ' << tA->name << "=\"" << tA->value << '"';
					tA = tA->next;
				}
			}
			cout << '>' << endl;
			if (node->child != nullptr)
			{
				Print(node->child, count + 1, printAttribute, true);
			}
			for (size_t i = 0; i < count; ++i)
			{
				cout << "·";
			}
			cout << "</" << node->tagName << '>' << endl;
			if (printBrother && node->brother != nullptr)
			{
				Print(node->brother, count, printAttribute, printBrother);
			}
		}
	}
	MLParser & MLParser::operator[](string tagName)
	{
		if (tagName == "") { now = root; return *this; }
		if (now->child != nullptr)
		{
			if (now->child->tagName == tagName)
			{
				now = now->child;
				return *this;
			}
			Node *tNode = now->child->brother;
			while (tNode != nullptr)
			{
				if (tNode->tagName == tagName)
				{
					now = tNode;
					return *this;
				}
				tNode = tNode->brother;
			}
		}
		SetErrMsg("Can't find <" + tagName + ">");
		return *this;
	}
	MLParser & MLParser::operator[](unsigned short n)
	{
		if (n == 0) { return *this; }
		unsigned short i = 0;
		Node *tNode = now->brother;
		while (tNode != nullptr)
		{
			if (tNode->tagName == now->tagName)
			{
				++i;
			}
			if (i == n)
			{
				now = tNode;
				return *this;
			}
			tNode = tNode->brother;
		}
		SetErrMsg("Can't find <" + now->tagName + ">[" + std::to_string(n) + "]");
		return *this;
	}
	Results MLParser::SearchByTagName(const string & name)
	{
		StrNode::iterator it;
		list<Result> result;
		for (it = sTagName.lower_bound(name); it != sTagName.upper_bound(name); ++it)
		{
			result.push_back(Result(raw, it->second));
		}
		return result;
	}
	Results MLParser::SearchByAttribute(const string & AttributeName)
	{
		StrNode::iterator it;
		list<Result> result;
		for (it = sAttribute.lower_bound(AttributeName); it != sAttribute.upper_bound(AttributeName); ++it)
		{
			result.push_back(Result(raw, it->second));
		}
		return result;
	}
	Results MLParser::SearchByAttribute(const string & AttributeName, const string & AttributeValue)
	{
		Results rs = SearchByAttribute(AttributeName), rs2;
		string atValue;
		for (auto result : rs)
		{
			result.FindAttribute(AttributeName, atValue);
			if(atValue == AttributeValue)
			{
				rs2.push_back(result);
			}
		}
		return rs2;
	}
}