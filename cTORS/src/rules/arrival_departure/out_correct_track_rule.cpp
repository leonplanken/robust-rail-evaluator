#include "BusinessRules.h"

/*

Rule that verifies that leaving shunting units leave over the correct tracks.

*/

pair<bool, string> out_correct_track_rule::IsValid(State* state, Action* action) const {
	if (ExitAction* ea = dynamic_cast<ExitAction*>(action)) {
		if (state->GetPosition(ea->GetShuntingUnit()) != ea->GetOutgoing()->GetParkingTrack())
			return make_pair(false, "Shunting unit " + ea->GetOutgoing()->GetShuntingUnit()->toString() + " should leave from track " + ea->GetOutgoing()->GetParkingTrack()->toString());
	}
	return make_pair(true, "");
}

