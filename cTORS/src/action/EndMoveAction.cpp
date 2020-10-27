#include "Action.h"
#include "State.h"

void EndMoveAction::Start(State* state) const {
	ShuntingUnit* su = GetShuntingUnit();
	state->SetMoving(su, false);
	state->SetPrevious(su, nullptr);
	state->AddActiveAction(su, this);
}

void EndMoveAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
}

string EndMoveAction::toString() const {
	return "EndMove " + su->toString();
}

void EndMoveActionGenerator::Generate(State* state, list<Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	for (auto su : sus) {
		auto track = state->GetPosition(su);
		if (state->IsMoving(su) && track->standingAllowed && !state->HasActiveAction(su)) {
			Action* a = new EndMoveAction(su, 25);
			out.push_back(a);
		}
	}
}