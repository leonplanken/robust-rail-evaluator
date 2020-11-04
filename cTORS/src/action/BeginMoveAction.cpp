#include "Action.h"
#include "State.h"

void BeginMoveAction::Start(State* state) const {
	state->SetMoving(GetShuntingUnit(), true);
	state->AddActiveAction(su, this);
	state->SetWaiting(su, false);
}

void BeginMoveAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
}

string BeginMoveAction::toString() const {
	return "BeginMove " + su->toString();
}

void BeginMoveActionGenerator::Generate(State* state, list<Action*>& out) const {
	//TODO check employee availability, add duration to the action for walking distance
	auto& sus = state->GetShuntingUnits();
	for (auto su : sus) {
		if (!state->IsWaiting(su) && !state->IsMoving(su) && !state->HasActiveAction(su)) {
			Action* a = new BeginMoveAction(su, su->GetStartUpTime(state->GetDirection(su)));
			out.push_back(a);
		}
	}
}