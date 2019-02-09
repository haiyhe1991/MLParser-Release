#pragma once
#ifndef USING_EXCEPTION_CYAN_MLPARSER
#define USING_EXCEPTION_CYAN_MLPARSER 1
#include "MLParser_exceptions.h"
#endif // !USING_EXCEPTION_CYAN_MLPARSER
#include "Node.hpp"
#include <string>
#include <map>
#include <list>
#include <regex>
using std::list;
using std::multimap;
using std::string;
using std::make_pair;
namespace Cyan
{
	typedef multimap<string, Node *> StrNode;
	typedef list<Result> Results;
	class MLParser
	{
	friend class Result;
	private:
		char *raw;//saving raw text
		Node *root;
		Node *now;//saving a location for query
		string errorMsg;
		bool okay;
		StrNode sTagName;//存储 <tagname,Node*> 元组，用于模糊查询
		StrNode sAttribute;//用于模糊搜索
		int *use_count;
		void Print(Cyan::Node * node, size_t count, bool printAttribute, bool printBrother);
		void preprocess(string &m);
		void AddTag(Node *node)
		{
			sTagName.insert(make_pair(node->tagName, node));
		}
		void AddAttribute(string name, Node *node)
		{
			sAttribute.insert(make_pair(name, node));
		}
		void SetErrMsg(string msg)
		{
			errorMsg = msg;
			okay = false;
		}
	public:
		MLParser() :raw(nullptr), root(nullptr), now(nullptr), errorMsg("") ,okay(true),use_count(new int(1)){}
		MLParser(const MLParser & MLP)
		{
			++*MLP.use_count;
			use_count = MLP.use_count;
			root = MLP.root;
			now = MLP.now;
			sTagName = MLP.sTagName;
			sAttribute = MLP.sAttribute;
			raw = MLP.raw;
		}
		MLParser & operator=(const MLParser & MLP)
		{
			++*MLP.use_count;
			if (--*use_count == 0)
			{
				delete[] raw;
				delete root;
				delete use_count;
			}
			use_count = MLP.use_count;
			root = MLP.root;
			now = MLP.now;
			sTagName = MLP.sTagName;
			sAttribute = MLP.sAttribute;
			raw = MLP.raw;
			return *this;
		}
		~MLParser()
		{
			if (--*use_count == 0)
			{
				delete[] raw;
				delete root;
				delete use_count;
			}
		}
		bool Parse(string html);
		void PrintTree(bool printAttributes = false);
		MLParser & operator[](const string & tagName);
		MLParser & operator[](unsigned short n);
		string GetTagName() 
		{
			if (!okay) return "";
			string t = now->tagName;
			now = root;
			return t;
		}
		//string GetAttribute(const string & AttributeName);
#if defined(USING_EXCEPTION_CYAN_MLPARSER) && (USING_EXCEPTION_CYAN_MLPARSER == 1)
		string GetAttribute(const string & AttributeName)
		{
			if (!okay) return "";
			string *ps = now->GetAttribute(AttributeName);
			if (ps == nullptr)
			{
				throw CantFindAttribute(AttributeName);
				return "";
			}
			now = root;
			return string(*ps);
		}
#endif // USING_EXCEPTION_CYAN_MLPARSER
		bool FindAttribute(const string & AttributeName,string & AttributeValue) 
		{
			if (!okay) return false;
			string *ps = now->GetAttribute(AttributeName);
			if (ps == nullptr)
			{
#if defined(USING_EXCEPTION_CYAN_MLPARSER) && (USING_EXCEPTION_CYAN_MLPARSER == 1)
				throw CantFindAttribute(AttributeName);
#endif // USING_EXCEPTION_CYAN_MLPARSER
				SetErrMsg("Can't find Attribute '" + AttributeName + "'");
				AttributeValue = "";
				return false;
			}
			AttributeValue = string(*ps);
			now = root;
			return true;
		}
		string GetContent()
		{
			if (!okay) return "";
			using std::regex;
			string t = substr(raw + now->txtOffset, now->count);
			regex exp("<([\\s\\S]+)>");
			t = regex_replace(t, exp, "");
			now = root;
			return t;
		}
		string GetInner()
		{
			if (!okay) return "";
			string t =  substr(raw + now->txtOffset, now->count);
			now = root;
			return t;
		}
		Results SearchByTagName(const string &name);
		Results SearchByAttribute(const string &AttributeName);
		Results SearchByAttribute(const string &AttributeName,const string &AttributeValue);
		string GetErrorMsg()
		{
			//不需要重置now的状态
			//在搜索时检测到okay为false
			//会自动重置now=root
			okay = true;//重置状态便于继续使用
			return errorMsg;
		}
		bool OK() const
		{
			return okay;
		}
	};
	class Result
	{
		friend class MLParser;
	private:
		MLParser *ml;
		Node *node;
		Result(MLParser *ml_, Node *node_) :ml(ml_), node(node_) {}
	public:
		MLParser &GetMLParser()
		{
			ml->now = node;
			return *ml;
		}
		string GetTagName()
		{
			return node->tagName;
		}
		bool FindAttribute(const string & AttributeName, string & AttributeValue)
		{
			ml->now = node;
			return ml->FindAttribute(AttributeName, AttributeValue);
		}
		string GetContent()
		{
			ml->now = node;
			return ml->GetContent();
		}
		string GetInner()
		{
			ml->now = node;
			return ml->GetInner();
		}
		~Result() {}
	};
}