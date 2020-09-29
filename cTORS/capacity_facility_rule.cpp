#include "BusinessRules.h"

/*

Rule that verifies that no more tasks are executed at a facility than the facility can handle.

*/

pair<bool, string> capacity_facility_rule::IsValid(State* state, Action* action) const {
	if (ServiceAction* sa = dynamic_cast<ServiceAction*>(action)) {
		auto fa = sa->GetFacility();
		auto& activeTasks = state->GetActiveTasks();
		auto& sus = state->GetShuntingUnits();
		int count = 0;
		for (auto su : sus) {
			auto& actions = state->GetActiveActions(su);
			for (auto a : actions) {
				if (ServiceAction* _sa = dynamic_cast<ServiceAction*>(a)) {
					if (_sa->GetFacility() == fa) 
						if(++count > fa->GetCapacity())
							return make_pair(false, fa->toString() + " is at its capacity and cannot service train " + sa->GetTrain()->toString() + " from " + sa->GetShuntingUnit()->toString()+".");
				}
			}
		}
	}
	return make_pair(true, "");
}

