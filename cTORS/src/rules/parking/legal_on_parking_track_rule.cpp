#include "BusinessRules.h"

/**
 * Validates an EndMoveAction or WaitAction for the given state. 
 * The EndMoveAction or WaitAction is invalid iff
 * 1. The current Track of the ShuntingUnit does not allow for parking.
 * @return A pair describing 1) whether the action is valid, and 2) if not, why
 */
pair<bool, string> legal_on_parking_track_rule::IsValid(const State* state, const Action* action) const {
	if(instanceof<EndMoveAction>(action) || instanceof<WaitAction>(action)) {
		auto su = action->GetShuntingUnit();
		auto position = state->GetPosition(su);
		if (!position->parkingAllowed)
			return make_pair(false, "Parking is not allowed on track " + position->toString() + ".");
	}
	return make_pair(true, "");
}

