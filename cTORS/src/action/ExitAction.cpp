#include "Action.h"
#include "State.h"

void ExitAction::Start(State* state) const {
	Track* parkingTrack = outgoing->GetParkingTrack();
	Track* sideTrack = outgoing->GetSideTrack();
	ShuntingUnit* su = GetShuntingUnit();
	if (!outgoing->IsInstanding())
		state->ReserveTracks(vector({ parkingTrack, sideTrack }));
	state->RemoveOutgoing(outgoing);
	state->AddActiveAction(su, this);
}

void ExitAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
	if (!outgoing->IsInstanding())
		state->FreeTracks(vector({ outgoing->GetParkingTrack(), outgoing->GetSideTrack() }));
	state->RemoveShuntingUnit(su);
}

string ExitAction::toString() const {
	return "Exit from " + su->toString() + " at " + outgoing->GetParkingTrack()->toString() + " at T" + to_string(outgoing->GetTime());
}

void ExitActionGenerator::Generate(State* state, list<Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	auto& outgoing = state->GetOutgoingTrains();
	if (outgoing.size() == 0) return;
	int minIndex = outgoing.at(0)->GetStandingIndex();
	for (auto ou : outgoing) minIndex = (ou->GetStandingIndex() < minIndex) ? ou->GetStandingIndex() : minIndex;
	for (auto ou : outgoing) {
		if (ou->IsInstanding()) {
			if (state->GetTime() < state->GetEndTime()) continue;
			if (ou->GetStandingIndex() > minIndex) continue;
		}
		for (auto su : sus) {
			if (state->HasActiveAction(su)) continue;
			if (su->GetNumberOfTrains() != ou->GetShuntingUnit()->GetNumberOfTrains()) continue;
			if (state->GetPosition(su) != ou->GetParkingTrack()) continue;
			Action* a = new ExitAction(su, 0, ou);
			out.push_back(a);
		}
	}
}