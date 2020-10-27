#pragma once
#include <exception>
#include <string>
using namespace std;

class ScenarioFailedException : public exception
{
private:
	const string message;
public:
	ScenarioFailedException() = default;
	ScenarioFailedException(string message) : message(" " + message) {};
	~ScenarioFailedException() = default;
	inline virtual const char* what() const throw()  {
		return ("The scenario failed."+ message).c_str();
	}
};

class InvalidLocationException : public exception
{
private:
	string message;
public:
	InvalidLocationException() = default;
	InvalidLocationException(string message) : message(" " + message) {};
	~InvalidLocationException() = default;
	inline virtual const char* what() const throw() {
		return ("The location specified is invalid." + message).c_str();
	}
};

class InvalidScenarioException : public exception
{
private:
	string message;
public:
	InvalidScenarioException() = default;
	InvalidScenarioException(string message) : message(" "+message) {};
	~InvalidScenarioException() = default;
	inline virtual const char* what() const throw() {
		return ("The scenario specified is invalid." + message).c_str();
	}
};

class InvalidConfigException :
	public exception
{
private:
	string message;
public:
	InvalidConfigException() = default;
	InvalidConfigException(string message) : message(" "+message) {};
	~InvalidConfigException() = default;
	inline virtual const char* what() const throw() {
		return ("The config file specified is invalid." + message).c_str();
	}
};

