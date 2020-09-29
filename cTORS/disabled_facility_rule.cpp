#include "BusinessRules.h"

/*

Rule that verifies that no tasks are assigned to facilities which are disabled by a disturbance.

*/

pair<bool, string> disabled_facility_rule::IsValid(State* state, Action* action) const {
	if (ServiceAction* sa = dynamic_cast<ServiceAction*>(action)) {
		
	}
	return make_pair(true, "");
}

