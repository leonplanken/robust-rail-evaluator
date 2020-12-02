#include "Action.h"
#include "State.h"

void CombineAction::Start(State* state) const {
    auto frontSU = GetFrontShuntingUnit();
    auto rearSU = GetRearShuntingUnit();
    auto suState = state->GetShuntingUnitState(frontSU);
    auto track = suState.position;
    auto previous = suState.previous;
    auto frontTrain = suState.frontTrain;
    state->RemoveShuntingUnit(frontSU);
    state->RemoveShuntingUnit(rearSU);
    auto combinedSU = new ShuntingUnit(*this->combinedSU);
    
    state->AddShuntingUnitOnPosition(combinedSU, track, previous, frontTrain, position);
    state->SetInNeutral(combinedSU, inNeutral);
    state->AddActiveAction(combinedSU, this);
}

void CombineAction::Finish(State* state) const {
    state->RemoveActiveAction(combinedSU, this);
}

const string CombineAction::toString() const {
	return "CombineAction " + GetCombinedShuntingUnit()->toString() + " from  "
        + GetFrontShuntingUnit()->GetTrainString() + " and " +GetRearShuntingUnit()->GetTrainString();
}

CombineAction::~CombineAction() {
	delete combinedSU;
}

void CombineActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	//TODO employees
	auto& sus = state->GetShuntingUnits();
    for (auto track : location->GetTracks()) {
        auto sus = state->GetOccupations(track);
        if(sus.size() < 2) continue;
        for(auto it=sus.begin(); it!=prev(sus.end()); it++) {
            auto suA = *it;
            auto suB = *next(it);
            auto& suStateA = state->GetShuntingUnitState(suA);
            auto& suStateB = state->GetShuntingUnitState(suB);
            if(suStateA.HasActiveAction() || suStateB.HasActiveAction()) continue;
            int position = static_cast<int>(distance(sus.begin(), it));
            // In case both shunting units are in neutral, they can be combined in both directions
            bool neutral = suStateA.inNeutral && suStateB.inNeutral;
            // In case both shunting units are not in neutral, they can only combine if they have the same direction
            bool notNeutral = (!suStateA.inNeutral && !suStateB.inNeutral && suStateA.previous == suStateB.previous);
            const ShuntingUnit *frontSU, *rearSU;
            int duration;
            // In case one shunting unit is in neutral, the other one becomes the front shunting unit
            if (neutral || (!suStateA.inNeutral && suStateB.inNeutral)
                || (notNeutral && track->IsASide(suStateA.previous))) {
                frontSU = suA;
                rearSU = suB;
                duration = suStateA.frontTrain->GetType()->combineDuration;
            } else if (neutral || (suStateA.inNeutral && !suStateB.inNeutral)
                || (notNeutral && track->IsBSide(suStateA.previous))) {
                frontSU = suB;
                rearSU = suA;
                duration = suStateB.frontTrain->GetType()->combineDuration;
            } else continue;
            auto frontTrains = copy_of(frontSU->GetTrains());
            auto rearTrains = copy_of(rearSU->GetTrains());
            vector<const Train*> combinedTrains(frontTrains);
            combinedTrains.insert(combinedTrains.end(), rearTrains.begin(), rearTrains.end());
            ShuntingUnit combinedSU(frontSU->GetID(), combinedTrains);
            auto combineAction = new CombineAction(frontSU, rearSU, combinedSU, track, duration, position, neutral);
			out.push_back(combineAction);
        }
    }
}