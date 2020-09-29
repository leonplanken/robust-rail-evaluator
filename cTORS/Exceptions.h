#pragma once
#include <exception>
#include <string>
using namespace std;

class ScenarioFailedException :
	public exception
{
private:
	const string message;
public:
	ScenarioFailedException() : message("The Scenario failed.") {};
	ScenarioFailedException(string message) : message("Scenario failed: " + message) {};
	~ScenarioFailedException() = default;
	inline const char* what() const throw()  {
		return message.c_str();
	};
};

class InvalidLocationException : 
	public exception
{
private:
	string message;
public:
	InvalidLocationException(string message);
	~InvalidLocationException();
	virtual const char* what() const throw();
};

class InvalidScenarioException :
	public exception
{
private:
	string message;
public:
	InvalidScenarioException(string message);
	~InvalidScenarioException();
	virtual const char* what() const throw();
};

class InvalidConfigException :
	public exception
{
private:
	string message;
public:
	InvalidConfigException(string message);
	~InvalidConfigException();
	virtual const char* what() const throw();
};

