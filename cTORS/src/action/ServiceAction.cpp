#include "Action.h"
#include "State.h"

void ServiceAction::Start(State* state) const {
	const ShuntingUnit* su = GetShuntingUnit();
	auto tu = GetTrain();
	auto ta = GetTask();
	state->AddActiveTaskToTrain(tu, ta);
	for (auto e : GetEmployees()) {
		//TODO
	}
	state->AddActiveAction(su, this);
}

void ServiceAction::Finish(State* state) const {
	auto tu = GetTrain();
	auto ta = GetTask();
	state->RemoveActiveAction(su, this);
	state->RemoveActiveTaskFromTrain(tu, ta);
	state->RemoveTaskFromTrain(tu, *ta);
}

const string ServiceAction::toString() const {
	return "Service task " + task->toString() + " to train " + GetTrain()->toString() + " of " + GetShuntingUnit()->toString()+  " at facility "+ facility->toString() ;
}

void ServiceActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	if(state->GetTime()==state->GetEndTime()) return;
	auto& sus = state->GetShuntingUnits();
	for (auto su : sus) {
		if (state->IsMoving(su) || state->IsWaiting(su) || state->HasActiveAction(su)) continue;
		auto tr = state->GetPosition(su);
		auto& fas = tr->GetFacilities();
		for (auto tu : su->GetTrains()) {
			for (const Task& task : state->GetTasksForTrain(tu)) {
				for (auto fa : fas) {
					Action* a = new ServiceAction(su, tu, &task, fa, vector<const Employee*> {});
					out.push_back(a);
				}
			}
		}
	}
}