#pragma once
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
		void SetErrMsg(string msg)
		{
			errorMsg = msg;
			okay = false;
		}
	public:
		MLParser() :raw(nullptr), root(nullptr), now(nullptr), errorMsg("") ,okay(true){}
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
		string GetTagName() 
		{
			if (!okay) return "";
			string t = now->tagName;
			now = root;
			return t;
		}
		bool FindAttribute(const string & AttributeName,string & AttributeValue) 
		{
			if (!okay) return false;
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