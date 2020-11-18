#include "BusinessRules.h"

/*

Rule that verifies that shunting units, upon starting a movement,
are not blocked on exit by other shunting units on their current track.

*/

pair<bool, string> blocked_first_track_rule::IsValid(const State* state, const Action* action) const {
	auto su = action->GetShuntingUnit();
	if (auto ma = dynamic_cast<const MoveAction*>(action)) {
		auto tracks = ma->GetTracks();
		auto start = tracks[1];
		auto next = tracks[2];
		auto& occ = state->GetOccupations(start);
		if (occ.size() > 1) {
			if (start->IsASide(next)) {
				if (occ.front() != su)
					return make_pair(false, "ShuntingUnit-" + su->toString() + " cannot leave Track " + start->toString() + " at the A-side to Track " + next->toString() + ": blocked.");
			}
			else {
				if (occ.back() != su)
					return make_pair(false, "ShuntingUnit-" + su->toString() + " cannot leave Track " + start->toString() + " at the B-side to Track " + next->toString() + ": blocked.");
			}
		}
	} else if (auto bma = dynamic_cast<const BeginMoveAction*>(action)) {
		auto start = state->GetPosition(su);		
		auto& occ = state->GetOccupations(start);
		if (occ.size() > 2) {
			if(occ.front() != su && occ.back() != su)
				return make_pair(false, "ShuntingUnit-" + su->toString() + " cannot leave Track " + start->toString() + ". Both sides blocked.");
		}
	}
	return make_pair(true, "");
}

