#include "Action.h"
#include "State.h"

void WaitAction::Start(State* state) const {
	state->SetWaiting(GetShuntingUnit(), true);
}

void WaitAction::Finish(State* state) const {
	state->SetWaiting(GetShuntingUnit(), false);
}

const string WaitAction::toString() const {
	return "Wait " + su->toString() + " for " + to_string(duration) + " seconds";
}

void WaitActionGenerator::Generate(const State* state, list<const Action*>& out) const {
	auto& sus = state->GetShuntingUnits();
	auto e = state->PeekEvent();
	if (e == nullptr || e->GetTime() == state->GetTime()) return;
	for (auto su : sus) {
		if (state->IsWaiting(su) || state->HasActiveAction(su)) continue;
		int dif = e->GetTime() - state->GetTime();
		if (dif > 30) dif = 30;
		Action* a = new WaitAction(su, dif);
		out.push_back(a);
	}
}