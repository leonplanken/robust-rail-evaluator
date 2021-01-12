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

const string BeginMoveAction::toString() const {
	return "BeginMove " + su->toString();
}

void BeginMoveActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	//TODO check employee availability, add duration to the action for walking distance
	if(state->GetTime()==state->GetEndTime()) return;
	for (const auto [su, suState] : state->GetShuntingUnitStates()) {
		if (!state->IsWaiting(su) && !state->IsMoving(su) && !state->HasActiveAction(su)) {
			Action* a = new BeginMoveAction(su, su->GetStartUpTime(state->GetFrontTrain(su)));
			out.push_back(a);
		}
	}
}