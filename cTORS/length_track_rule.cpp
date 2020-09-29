#include "BusinessRules.h"

/*

Rule that verifies that shunting units on a single track do not take up more 
space than available on that track.

*/

pair<bool, string> length_track_rule::IsValid(State* state, Action* action) const {
	Track* track;
	ShuntingUnit* su = action->GetShuntingUnit();
	bool move = false;
	if (ArriveAction* aa = dynamic_cast<ArriveAction*>(action)) {
		track = aa->GetDestinationTrack();
	} else if (MoveAction* ma = dynamic_cast<MoveAction*>(action)) {
		track = ma->GetDestinationTrack();
		move = true;
	} else if (state->IsMoving(su) && (dynamic_cast<WaitAction*>(action) || dynamic_cast<EndMoveAction*>(action))) {
		track = state->GetPosition(su);
	} else {
		return make_pair(true, "");
	}
	auto& occ = state->GetOccupations(track);
	double length = su->GetLength();
	for (auto& u : occ) {
		length += u->GetLength();
	}
	if (length > track->length) {
		if(!move || occ.size() > 0)
			return make_pair(false, "Adding ShuntingUnit-"+su->toString() + " to Track " + track->toString() + " exceeds the maximum length (" + to_string(length) + " > " +to_string(track->length) + ")");
	}
	return make_pair(true, "");
}

