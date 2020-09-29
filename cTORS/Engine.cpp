#include <list>
#include "Engine.h"
using namespace std;

Engine::Engine(const string &path) : path(path), location(Location(path)), 
	originalScenario(Scenario(path, location)), config(Config(path)),
	actionValidator(ActionValidator(&config)), actionManager(ActionManager(&config)) {}


Engine::~Engine() {
	TrainUnitType::types.clear();
	stateActionMap.clear();
}

list<Action*> &Engine::GetActions(State * state, Scenario * scenario) {
	ExecuteImmediateEvents(state);
	list<Action*>& actions = GetValidActions(state);
	debug_out("Got valid actions succesfully");
	EventQueue disturbances = scenario ? scenario->GetDisturbances() : EventQueue();
	while (actions.empty() && state->GetNumberOfEvents() > 0) {
		debug_out("No actions but still events available");
		Event* evnt;
		if (disturbances.size() > 0 && disturbances.top()->GetTime() <= state->PeekEvent()->GetTime())
			evnt = disturbances.top();
		else
			evnt = state->PopEvent();
		if (evnt->GetTime() != state->GetTime() && state->IsAnyInactive())
			throw ScenarioFailedException();
		ExecuteEvent(state, evnt);
		ExecuteImmediateEvents(state);
		if (state->GetTime() > state->GetEndTime()) {
			if ((state->GetIncomingTrains().size() + state->GetOutgoingTrains().size()) > 0) {
				throw ScenarioFailedException();
			} else {
				DELETE_LIST(actions)
			}
		} else {
			actions = GetValidActions(state);
		}
	}
	debug_out("Done getting actions. Found " << to_string(actions.size()) << " actions.");
	return actions;
}

void Engine::ApplyAction(State* state, Action* action) {
	debug_out("\tApplying action " + action->toString());
	state->StartAction(action);
}

list<Action*> &Engine::GetValidActions(State* state) {
	debug_out("Starting GetValidActions");
	if (state->IsChanged()) {
		auto& actions = stateActionMap[state];
		DELETE_LIST(actions)
		actionManager.Generate(state, actions);
		debug_out("Generated "+ to_string(actions.size())+" actions");
		actionValidator.FilterValid(state, actions);
		debug_out("GetValidActions list filtered");
		state->SetUnchanged();
	}
	return stateActionMap[state];
}

void Engine::ExecuteImmediateEvents(State* state) {
	if (state == nullptr) {
		throw exception("state == null, something went wrong");
	}
	debug_out("Execute immediate events (" << to_string(state->GetNumberOfEvents()) << " events queued)");
	while (state->GetNumberOfEvents() > 0) {
		Event *evnt = state->PeekEvent();
		debug_out("Next event at T=" << to_string(evnt->GetTime()) << ": " << evnt->toString());
		if (evnt->GetTime() > state->GetTime()) break;
		evnt = state->PopEvent();
		ExecuteEvent(state, evnt);
	}
}

void Engine::ExecuteEvent(State* state, Event* e) {
	Action* a = e->GetAction();
	if (a != nullptr) {
		//auto result = actionValidator.IsValid(state, a);
		//if (!result.first)
		//	throw ScenarioFailedException(result.second);
		debug_out("\tFinishing action " + a->toString());
		state->FinishAction(a);
	}
	state->SetTime(e->GetTime());
}

State* Engine::StartSession(const Scenario& scenario) {
	State* state = new State(scenario);
	stateActionMap[state];
	return state;
}

void Engine::EndSession(State* state) {
	auto actions = stateActionMap[state];
	actions.clear();
	stateActionMap.erase(state);
	delete state;
}