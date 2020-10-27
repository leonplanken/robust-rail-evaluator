#include "BusinessRules.h"

/*

Rule that verifies that leaving shunting units leave at the correct time.

*/

pair<bool, string> out_correct_time_rule::IsValid(State* state, Action* action) const {
	if (ExitAction* ea = dynamic_cast<ExitAction*>(action)) {
		if(state->GetTime() != ea->GetOutgoing()->GetTime())
			return make_pair(false, "Shunting unit " + ea->GetOutgoing()->GetShuntingUnit()->toString() + " should leave at time " + to_string(ea->GetOutgoing()->GetTime()) );
	}
	return make_pair(true, "");
}

