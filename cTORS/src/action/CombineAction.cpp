#include "Action.h"
#include "State.h"

void CombineAction::Start(State* state) const {
    auto frontSU = GetFrontShuntingUnit();
    auto rearSU = GetRearShuntingUnit();
    auto suState = state->GetShuntingUnitState(frontSU);
    auto track = suState.position;
    auto previous = suState.previous;
    bool direction = track->IsASide(previous);
    auto frontTrain = direction ? &combinedSU.GetTrains().front() : &combinedSU.GetTrains().back();
    state->RemoveShuntingUnit(frontSU);
    state->RemoveShuntingUnit(rearSU);
    state->AddShuntingUnitOnPosition(&combinedSU, track, previous, frontTrain, position);
    state->SetInNeutral(&combinedSU, inNeutral);
    state->AddActiveAction(&combinedSU, this);
}

void CombineAction::Finish(State* state) const {
    state->RemoveActiveAction(&combinedSU, this);
}

const string CombineAction::toString() const {
	return "CombineAction " + GetCombinedShuntingUnit()->toString() + " from  " + suString;
}

const Action* CombineActionGenerator::Generate(const State* state, const SimpleAction& action) const {
	auto combine = static_cast<const Combine*>(&action);
	auto frontSU = state->GetShuntingUnitByTrainIDs(action.GetTrainIDs());
    auto rearSU = state->GetShuntingUnitByTrainIDs(combine->GetSecondTrainIDs());
    auto frontState = state->GetShuntingUnitState(frontSU);
    auto rearState = state->GetShuntingUnitState(rearSU);
    auto& frontTrains = frontSU->GetTrains();
    auto& rearTrains = rearSU->GetTrains();
    auto duration = frontState.frontTrain->GetType()->combineDuration;
    auto track = frontState.position;
    auto position = state->GetPositionOnTrack(frontSU);
    auto neutral = frontState.inNeutral && rearState.inNeutral;
    bool front = *frontState.frontTrain == frontTrains.at(0);
    bool direction = track->IsASide(frontState.previous);
    if(front == direction) {
        vector<Train> combinedTrains(rearTrains);
        combinedTrains.insert(combinedTrains.end(), frontTrains.begin(), frontTrains.end());
        ShuntingUnit combinedSU(frontSU->GetID(), combinedTrains);
        return new CombineAction(frontSU, rearSU, combinedSU, track, duration, neutral, position);
    } else {
        vector<Train> combinedTrains(rearTrains.rbegin(), rearTrains.rend());
        combinedTrains.insert(combinedTrains.end(), frontTrains.rbegin(), frontTrains.rend());
        ShuntingUnit combinedSU(frontSU->GetID(), combinedTrains);
        return new CombineAction(frontSU, rearSU, combinedSU, track, duration, neutral, position);
    }
}

void CombineActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	if(state->GetTime()==state->GetEndTime()) return;
    //TODO employees
    for (auto track : location->GetTracks()) {
        auto sus = state->GetOccupations(track);
        if(sus.size() < 2) continue;
        for(auto it=sus.begin(); it!=prev(sus.end()); it++) {
            auto suA = *it;
            auto suB = *next(it);
            auto& suStateA = state->GetShuntingUnitState(suA);
            auto& suStateB = state->GetShuntingUnitState(suB);
            if(suStateA.HasActiveAction() || suStateB.HasActiveAction() || suStateA.moving || suStateB.moving || (suStateA.waiting && suStateB.waiting)) continue;
            // In case both shunting units are in neutral, they can be combined in both directions
            bool neutral = suStateA.inNeutral && suStateB.inNeutral;
            // In case both shunting units are not in neutral, they can only combine if they have the same direction
            bool notNeutral = (!suStateA.inNeutral && !suStateB.inNeutral && suStateA.previous == suStateB.previous);
            
            // In case one shunting unit is in neutral, the other one becomes the front shunting unit
            if(notNeutral && suStateA.previous != suStateB.previous) continue;
			out.push_back(Generate(state, Combine(suA, suB)));
        }
    }
}

const string Combine::GetSecondTrainsToString() const {
    return "[" + Join(secondTrainIDs.begin(), secondTrainIDs.end(), ", ") + "]";
}