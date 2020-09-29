#pragma once
#include <list>
#include <map>
#include "Utils.h"
#include "State.h"
#include "Scenario.h"
#include "Action.h"
#include "Event.h"
#include "Location.h"
#include "Exceptions.h"
#include "Config.h"

using namespace std;

class Engine
{
private:
	string path;
	Location location;
	Scenario originalScenario;
	Config config;
	ActionValidator actionValidator;
	ActionManager actionManager;
	map<State*, list<Action*>> stateActionMap;

	list<Action*> &GetValidActions(State* state);
	void ExecuteEvent(State* state, Event* e);
	void ExecuteImmediateEvents(State * state);
public:
	Engine() = delete;
	Engine(const string &path);
	~Engine();
	list<Action*> &GetActions(State* state, Scenario* scenario = nullptr);
	void ApplyAction(State* state, Action* action);
	State *StartSession(const Scenario& scenario);
	inline State *StartSession() { return StartSession(originalScenario); }
	void EndSession(State* state);
	inline const Location &GetLocation() const { return location; }
	inline const Scenario& GetScenario() const { return originalScenario; }
};

