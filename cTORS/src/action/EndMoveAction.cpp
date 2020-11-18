#include "Action.h"
#include "State.h"

void EndMoveAction::Start(State* state) const {
	const ShuntingUnit* su = GetShuntingUnit();
	state->SetMoving(su, false);
	state->SetInNeutral(su, true);
	state->AddActiveAction(su, this);
}

void EndMoveAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
}

const string EndMoveAction::toString() const {
	return "EndMove " + su->toString();
}

void EndMoveActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	for (auto su : sus) {
		auto track = state->GetPosition(su);
		if (state->IsMoving(su) && track->standingAllowed && !state->HasActiveAction(su)) {
			Action* a = new EndMoveAction(su, su->GetStartUpTime(state->GetFrontTrain(su)));
			out.push_back(a);
		}
	}
}