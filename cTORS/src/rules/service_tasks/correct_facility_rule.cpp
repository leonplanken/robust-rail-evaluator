#include "BusinessRules.h"

/*

Rule that verifies that service tasks are executed at the correct facility.

*/

pair<bool, string> correct_facility_rule::IsValid(State* state, Action* action) const {
	if (ServiceAction* sa = dynamic_cast<ServiceAction*>(action)) {
		auto task = sa->GetTask();
		auto faci = sa->GetFacility();
		if (!faci->ExecutesTask(task))
			return make_pair(false, faci->toString() + " cannot perform task " + task->toString() + ".");
	}
	return make_pair(true, "");
}

