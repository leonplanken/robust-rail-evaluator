#include "Action.h"
#include "State.h"

void ServiceAction::Start(State* state) const {
	ShuntingUnit* su = GetShuntingUnit();
	auto tu = GetTrain();
	auto ta = GetTask();
	state->RemoveTaskFromTrain(tu, *ta);
	state->AddActiveTaskToTrain(tu, ta);
	for (auto e : GetEmployees()) {
		//TODO
	}
	state->AddActiveAction(su, this);
}

void ServiceAction::Finish(State* state) const {
	state->RemoveActiveAction(su, this);
	state->RemoveActiveTaskFromTrain(GetTrain(), GetTask());
}

string ServiceAction::toString() const {
	return "Service task " + task->toString() + " to train " + GetTrain()->toString() + " of " + GetShuntingUnit()->toString()+  " at facility "+ facility->toString() ;
}

void ServiceActionGenerator::Generate(State* state, list<Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	for (auto su : sus) {
		if (state->IsMoving(su) || state->IsWaiting(su) || state->HasActiveAction(su)) continue;
		Track* tr = state->GetPosition(su);
		auto fas = tr->GetFacilities();
		for (Train* tu : su->GetTrains()) {
			for (Task& task : state->GetTasksForTrain(tu)) {
				for (Facility* fa : fas) {
					Action* a = new ServiceAction(su, tu, &task, fa, vector<Employee*> {});
					out.push_back(a);
				}
			}
		}
	}
}