#pragma once
#include "Node.hpp"
#include <string>
#include <map>
#include <list>
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
	private:
		char *raw;//saving raw text
		Node *root;
		Node *now;//saving a location for query
		string errorMsg;
		StrNode sTagName;//存储 <tagname,Node*> 元组，用于模糊查询
		StrNode sAttribute;//用于模糊搜索
		void Print(Cyan::Node * node, size_t count, bool printAttribute, bool printBrother);
		void preprocess(string &m);
		void Copy(MLParser & MLP)
		{
			if (MLP.raw == this->raw) return;
			this->Dispose();

			this->raw = Cyan::strcpy(MLP.raw);

			this->root = MLP.root->Copy(nullptr);
			this->now = root;
			this->errorMsg = nullptr;
		}
		void AddTag(Node *node)
		{
			sTagName.insert(make_pair(node->tagName, node));
		}
		void AddAttribute(string name, Node *node)
		{
			sAttribute.insert(make_pair(name, node));
		}
	public:
		MLParser() :raw(nullptr), root(nullptr), now(nullptr), errorMsg("") {}
		MLParser(MLParser & MLP)
		{
			Copy(MLP);
		}
		MLParser & operator=(MLParser & MLP)
		{
			Copy(MLP);
			return *this;
		}
		~MLParser() {}
		bool Parse(string html);
		void PrintTree(bool printAttributes = false);
		MLParser & operator[](string tagName);
		MLParser & operator[](unsigned short n);
		MLParser & XPath(string xpath);
		string GetTagName() 
		{
			string t = now->tagName;
			now = root;
			return t;
		}
		bool FindAttribute(const string & AttributeName,string & AttributeValue) 
		{
			string *ps = now->GetAttribute(AttributeName);
			if (ps == nullptr)
			{
				SetErrMsg("Can't find Attribute '" + AttributeName + "'");
				AttributeValue = "";
				return false;
			}
			AttributeValue = string(*ps);
			now = root;
			return true;
		}
		string GetContent() const;
		string GetInner()
		{
			string t =  substr(raw + now->txtOffset, now->count);
			now = root;
			return t;
		}
		Results SearchByTagName(const string &name);
		Results SearchByAttribute(const string &AttributeName);
		Results SearchByAttribute(const string &AttributeName,const string &AttributeValue);
		string GetErrorMsg() const
		{
			return errorMsg;
		}
		void SetErrMsg(string msg)
		{
			errorMsg = msg;
		}
		void Dispose()
		{
			delete[] raw;
			delete root;
		}
	};
	class Result
	{
		friend class MLParser;
	private:
		char *raw;
		Node *node;
		Result(char *raw_, Node *node_) :raw(raw_), node(node_) {}
	public:
		string GetTagName()
		{
			return node->tagName;
		}
		bool FindAttribute(const string & AttributeName, string & AttributeValue)
		{
			string *ps = node->GetAttribute(AttributeName);
			if (ps == nullptr)
			{
				AttributeValue = "";
				return false;
			}
			AttributeValue = string(*ps);
			return true;
		}
		string GetContent() const;
		string GetInner()
		{
			return substr(raw + node->txtOffset, node->count);
		}
		~Result() {}
	};
}