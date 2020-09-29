#include "Exceptions.h"
#include <string>


InvalidConfigException::InvalidConfigException(string message) : message(message) {}


InvalidConfigException::~InvalidConfigException() { }

const char* InvalidConfigException::what() const throw() {
	return "The config file specified is invalid.";
}
