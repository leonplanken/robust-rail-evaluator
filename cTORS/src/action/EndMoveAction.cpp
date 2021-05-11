#include "Action.h"
#include "State.h"

void EndMoveAction::Start(State* state) const {
	const ShuntingUnit* su = GetShuntingUnit();
	state->SetMoving(su, false);
	state->SetInNeutral(su, true);
	// TODO by setting the su in neutral, and followig up a BeginMoveAction you can do
	// a setback with time cost 0. This should not be allowed. Redesign EndMoveAction and SetBackAction
	state->AddActiveAction(su, this);
}

void EndMoveAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
}

const string EndMoveAction::toString() const {
	return "EndMove " + su->toString();
}

const Action* EndMoveActionGenerator::Generate(const State* state, const SimpleAction& action) const {
	auto su = state->GetShuntingUnitByTrainIDs(action.GetTrainIDs());
	auto suState = state->GetShuntingUnitState(su);
	return new EndMoveAction(su, su->GetStartUpTime(suState.frontTrain));
}

void EndMoveActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	for (auto& [su, suState]: state->GetShuntingUnitStates()) {
		auto& track = suState.position;
		if (suState.moving && !suState.beginMoving && !suState.waiting && track->parkingAllowed && !suState.HasActiveAction()) {
			out.push_back(Generate(state, EndMove(su)));
		}
	}
}