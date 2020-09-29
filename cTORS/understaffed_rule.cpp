#include "BusinessRules.h"

/*

Rule that verifies that all tasks have enough employees assigned, with the right skills,
such that the task will have all of its required skills available.

*/

pair<bool, string> understaffed_rule::IsValid(State* state, Action* action) const {
	//TODO
	return make_pair(true, "");
}

