#include "Action.h"
#include "State.h"

void SplitAction::Start(State* state) const {
	auto su = GetShuntingUnit();
	auto track = state->GetPosition(su);
	auto frontTrain = state->GetFrontTrain(su);
	int positionOnTrack = state->GetPositionOnTrack(su);
	// ShuntingUnit* su1 = new ShuntingUnit(su->GetID(), vector<Train*>(su->GetTrains().begin(), su.GetTrains()->begin() + splitAt));
	// ShuntingUnit* su2 = new ShuntingUnit(su->GetID()+1, vector<Train*>(su->GetTrains().begin()+splitAt, su->GetTrains().end()));
	// if(direction)
	// state->RemoveShuntingUnit(su);
	// state->OccupyTrack(su1, track);
	// state->AddActiveAction(su1, this);
	// state->AddActiveAction(su2, this);
}

void SplitAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
}

const string SplitAction::toString() const {
	return "SplitAction";
}

void SplitActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	for (auto su : sus) {
		if (state->IsMoving(su) || state->IsWaiting(su) || state->HasActiveAction(su)) continue;
		auto tr = state->GetPosition(su);
		
	}
}