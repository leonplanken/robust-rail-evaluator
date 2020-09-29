#include "Exceptions.h"



ScenarioFailedException::ScenarioFailedException()
{
}


ScenarioFailedException::~ScenarioFailedException()
{
}

const char* ScenarioFailedException::what() const throw() {
	return message.c_str();
}
