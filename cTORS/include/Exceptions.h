/** \file Exceptions.h
 * Defines the domain specific exceptions
 */
#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
//!\cond SYS_HEADER
#include <exception>
#include <string>
//!\endcond
using namespace std;

#ifndef DEFINE_EXCEPTION
/** MACRO for defining domain specific subclasses to prevent duplicate code */
#define DEFINE_EXCEPTION(name, exp_str) \
class name : public exception \
{ \
private: \
	const string message; \
public: \
	/** Construct a name with no extra info */\
	name() = default; \
	/** Construct a name with extra info */\
	name(const string& message) : message(string(exp_str) + " " + message) {}; \
	/** name's destructor */\
	~name() = default; \
	/** Get the error message */\
	inline virtual const char* what() const throw() override { \
		return message.c_str(); \
	} \
};
#endif

/** Raise this exception when a TORS session ends in an invalid, or finished but unsolved State */
DEFINE_EXCEPTION(ScenarioFailedException, 	"The scenario failed.")
/** Raise this exception when the specified Location file is invalid */
DEFINE_EXCEPTION(InvalidLocationException,	"The location specified is invalid.")
/** Raise this exception when the specified Scenario file is invalid */
DEFINE_EXCEPTION(InvalidScenarioException,	"The scenario specified is invalid.")
/** Raise this exception when the specified Config file is invalid */
DEFINE_EXCEPTION(InvalidConfigException, 	"The config file specified is invalid.")
/** Raise this exception when trying to apply an invalid Action */
DEFINE_EXCEPTION(InvalidActionException, 	"The action is invalid.")
/** Raise this exception when trying to find a non-existing Path in the shunting yard */
DEFINE_EXCEPTION(NonExistingPathException, 	"The path does not exist.")

#endif