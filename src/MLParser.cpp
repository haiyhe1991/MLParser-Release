#include "MLParser.h"
#include "Scanner.hpp"
#include <stack>
#include <regex>
#include <list>
#ifndef USING_EXCEPTION_CYAN_MLPARSER
#define USING_EXCEPTION_CYAN_MLPARSER 1
#include "MLParser_exceptions.h"
#endif // !USING_EXCEPTION_CYAN_MLPARSER
using std::list;
using std::stack;
using std::string;
namespace Cyan
{
	void MLParser::preprocess(string &m)
	{
		//���´�������ȥ��script��ǩ��style��ǩ��ע��
		std::regex exp1("<script([\\S\\s]+?)<\\/script>");
		m = std::regex_replace(m, exp1, "");
		std::regex exp2("<!--([\\S\\s]+?)-->");
		m = std::regex_replace(m, exp2, "");
		std::regex exp3("<!([\\S\\s]+?)>");
		m = std::regex_replace(m, exp3, "");
		std::regex exp4("<style([\\S\\s]+?)<\\/style>");
		m = std::regex_replace(m, exp4, "");

		raw = Cyan::strcpy(m.data());
		//���´�������ȥ��\n \r \t
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
		if (root != nullptr)
		{
			if (*use_count == 1)
			{
				delete[] raw;
				delete root;
				delete use_count;
				use_count = new int(1);
				sTagName.clear();
				sAttribute.clear();
			}
			else
			{
				--*use_count;
			}
		}

		preprocess(html);
		//initial Scanner & Scan
		Scanner SC = Scanner(raw);
		SC.Scan();

		stack<Node *> NodeStack;
		list<Node *> NodeList;
		root = new Node();
		root->tagName = "_root_";
		Node *lNode = root;//��¼��һ����������Node
		Node *pNode = root;//��һ���½ڵ�ĸ��׽ڵ�
		Node *tNode = root;//���ڸ���Node��Ϣ
		NodeStack.push(root);
		Attribute *lAttribute = nullptr;//��¼��һ����������Attribute;

		//****************************************
		//��Ϊ��ƴ��󣬲��ò�ʹ�øñ�����¼һЩ��Ϣ
		//����LeftAngleBracket��EndTag��ı����ֵ����ΪL_A_B����EndTag��
		//����RightAngleBracket��ʹ�����ֵ��
		//����ñ���(tType) == LeftAngleBracket
		//��ôRightAngleBracket���޸�lNode��txtOffset�ֶ�Ϊ�������offset
		//��֮�����κβ���
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
				AddTag(tNode);//����ģ������
				NodeStack.push(tNode);
				NodeList.push_back(tNode);
				tType = LeftAngleBracket;//��tType����ǰע��
				break;
			case AttributeName:
				lAttribute = lNode->AddAttribute(token->value, "");
				AddAttribute(token->value, lNode);//����ģ������
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
						//���˵tNode->tagName != token->value���Ͳ������it == NodeList.end()
						//˵������ƾ�ճ��ֵ�һ��End_Tag
						//���ƾ�ճ��ֵ�End_Tag��Ӧ������κ�Ӱ��
						//�������ǰѵ�������Node�ټӻ�ȥ
						NodeStack.push(tNode);
						NodeList.push_back(tNode);
						break;
					}
					//����Ĵ��������޸�parent��ָ�����
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
				tType = EndTag;//��tType����ǰע��
				break;
			case RightAngleBracket:
				if (tType == LeftAngleBracket)//��tType����������ע��
				{
					lNode->txtOffset = token->offset + 1;//token->offsetָ���Ҽ����ţ�+1��Ϊָ�����ݵĵ�һ���ַ�
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
				cout << "��";
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
				cout << "��";
			}
			cout << "</" << node->tagName << '>' << endl;
			if (printBrother && node->brother != nullptr)
			{
				Print(node->brother, count, printAttribute, printBrother);
			}
		}
	}
	MLParser & MLParser::operator[](const string & tagName)
	{
		if ( tagName == "" || (!okay) ) { now = root; return *this; }
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
#if defined(USING_EXCEPTION_CYAN_MLPARSER) && (USING_EXCEPTION_CYAN_MLPARSER == 1)
		throw CantFindTag(tagName);
#endif // USING_EXCEPTION_CYAN_MLPARSER
		SetErrMsg("Can't find <" + tagName + ">");
		return *this;
	}
	MLParser & MLParser::operator[](unsigned short n)
	{
		if (n == 0 || (!okay)) { now = root;  return *this; }
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
#if defined(USING_EXCEPTION_CYAN_MLPARSER) && (USING_EXCEPTION_CYAN_MLPARSER == 1)
		throw CantFindTag(now->tagName + "[" + std::to_string(n) + "]");
#endif // USING_EXCEPTION_CYAN_MLPARSER
		SetErrMsg("Can't find <" + now->tagName + ">[" + std::to_string(n) + "]");
		return *this;
	}
	Results MLParser::SearchByTagName(const string & name)
	{
		StrNode::iterator it;
		list<Result> result;
		for (it = sTagName.lower_bound(name); it != sTagName.upper_bound(name); ++it)
		{
			result.push_back(Result(this, it->second));
		}
		return result;
	}
	Results MLParser::SearchByAttribute(const string & AttributeName)
	{
		StrNode::iterator it;
		list<Result> result;
		for (it = sAttribute.lower_bound(AttributeName); it != sAttribute.upper_bound(AttributeName); ++it)
		{
			result.push_back(Result(this, it->second));
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