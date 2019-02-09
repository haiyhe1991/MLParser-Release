#pragma once
#include <exception>
#include <string>
using std::string;
using std::exception;
class CantFindTag : public exception 
{
public:
	CantFindTag(const string &tagName):msg("")
	{
		msg += "Can't find tag '";
		msg += tagName;
		msg += "' .";
	}
	const char * what() const throw () 
	{
		return msg.data();
	}
private:
	string msg;
};
class CantFindAttribute : public exception
{
public:
	CantFindAttribute(const string &AttributeName) :msg("")
	{
		msg += "Can't find Attribute '";
		msg += AttributeName;
		msg += "' .";
	}
	const char * what() const throw ()
	{
		return msg.data();
	}
private:
	string msg;
};