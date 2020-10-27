#include "BusinessRules.h"

/*

Rule that verifies if a shunting unit is parked on a track where setback is allowed.

*/

pair<bool, string> legal_on_setback_track_rule::IsValid(State* state, Action* action) const {
	auto su = action->GetShuntingUnit();
	bool stopping = false;
	if (EndMoveAction* ema = dynamic_cast<EndMoveAction*>(action)) stopping = true;
	else if (WaitAction* wa = dynamic_cast<WaitAction*>(action)) stopping = true;
	if (stopping) {
		Track* position = state->GetPosition(su);
		if (!position->sawMovementAllowed)
			return make_pair(false, "Parking is not allowed on track " + position->toString() + ".");
	}
	return make_pair(true, "");
}

