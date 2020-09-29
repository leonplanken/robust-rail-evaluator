#include "Exceptions.h"
#include <string>


InvalidLocationException::InvalidLocationException(string message) : message(message) {}


InvalidLocationException::~InvalidLocationException() { }

const char* InvalidLocationException::what() const throw() {
	return "The location specified is invalid.";
}
