#include "Action.h"
#include "State.h"

void SetbackAction::Start(State* state) const {
	ShuntingUnit* su = GetShuntingUnit();
	state->SetPrevious(su, nullptr);
	for (auto e : GetDrivers()) {
		//TODO
	}
	state->AddActiveAction(su, this);
}

void SetbackAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
}

string SetbackAction::toString() const {
	return "Setback " + GetShuntingUnit()->toString();
}


SetbackActionGenerator::SetbackActionGenerator(const json& params) : ActionGenerator(params) {
	params.at("constant_time").get_to(constantTime);
	params.at("default_time").get_to(defaultTime);
	params.at("norm_time").get_to(normTime);
	params.at("walk_time").get_to(walkTime);
}

int SetbackActionGenerator::GetDuration(State* state, ShuntingUnit* su, int numDrivers) const {
	if(defaultTime) return su->GetSetbackTime(true, numDrivers < 2, state->GetDirection(su));
	return su->GetSetbackTime(normTime, walkTime, state->GetDirection(su), constantTime);
}

void SetbackActionGenerator::Generate(State* state, list<Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	bool driver_mandatory = false;//TODO get value from config
	for (auto su : sus) {
		if (!state->IsMoving(su) || state->IsWaiting(su) || state->HasActiveAction(su)) continue;
		Track* tr = state->GetPosition(su);
		vector<Employee*> drivers;
		if (driver_mandatory) {
			//TODO
		}
		int duration = GetDuration(state, su, drivers.size());
		Action* a = new SetbackAction(su, drivers, duration);
		out.push_back(a);		
	}
}