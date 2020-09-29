#include "Exceptions.h"
#include <string>


InvalidScenarioException::InvalidScenarioException(string message) : message(message) {}


InvalidScenarioException::~InvalidScenarioException() { }

const char* InvalidScenarioException::what() const throw() {
	return "The scenario specified is invalid.";
}
