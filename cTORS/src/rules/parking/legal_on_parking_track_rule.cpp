#include "BusinessRules.h"

/*

Rule that verifies that parked shunting units are on a track where parking is allowed.

*/

pair<bool, string> legal_on_parking_track_rule::IsValid(State* state, Action* action) const {
	auto su = action->GetShuntingUnit();
	bool stopping = false;
	if (EndMoveAction* ema = dynamic_cast<EndMoveAction*>(action)) stopping = true;
	else if (WaitAction* wa = dynamic_cast<WaitAction*>(action)) stopping = true; 
	if(stopping) {
		Track* position = state->GetPosition(su);
		if (!position->parkingAllowed)
			return make_pair(false, "Parking is not allowed on track " + position->toString() + ".");
	}
	return make_pair(true, "");
}

