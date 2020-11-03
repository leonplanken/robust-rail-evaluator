#pragma once
#include <list>
#include <vector>
#include "ShuntingUnit.h"
#include "TrainGoals.h"
#include "Track.h"
#include "Employee.h"
#include "Config.h"

#ifndef ACTION_OVERRIDE
#define ACTION_OVERRIDE(name) \
	void Start(State* state) const override; \
	void Finish(State* state) const override; \
	inline name* clone() const override { return new name(*this); } \
	string toString() const override;
#endif

using namespace std;

class ShuntingUnit;
class Track;
class BusinessRule;
class State;

class Action
{
private:
	static int newUID;
protected:
	const int uid;
	ShuntingUnit* su;
	int duration;
public:
	Action() = delete;
	Action(ShuntingUnit* su, int duration) : uid(newUID++), su(su), duration(duration) {};
	Action(ShuntingUnit* su) : Action(su, -1) { };
	Action(const Action& action) = default;
	virtual ~Action() = default;
	inline void SetDuration(int d) { duration = d; }
	inline int GetDuration() const { return duration; }
	inline ShuntingUnit* GetShuntingUnit() const { return su; }
	virtual void Start(State* state) const = 0;
	virtual void Finish(State* state) const = 0;
	virtual Action* clone() const = 0;
	virtual string toString() const = 0;
	inline const bool IsEqual(const Action& a) const { return uid == a.uid; }
	inline virtual bool operator==(const Action& a) const { return IsEqual(a); }
	inline virtual bool operator!=(const Action& a) const { return !IsEqual(a); }
	inline virtual const vector<Track*> GetReservedTracks() const { return {}; }
};

class ArriveAction : public Action { 
private:
	Incoming* incoming;
public:
	ArriveAction() = delete;
	ArriveAction(ShuntingUnit* su, int duration, Incoming* incoming) : Action(su, duration), incoming(incoming) {};
	ArriveAction(ShuntingUnit* su, Incoming* incoming) : Action(su), incoming(incoming) {};
	inline Track* GetDestinationTrack() const { return incoming->GetParkingTrack(); }
	inline const vector<Track*> GetReservedTracks() const override { return (incoming->IsInstanding() ? vector<Track*>() : vector<Track*>({GetDestinationTrack()})); }
	inline Incoming* GetIncoming() const { return incoming; }
	ACTION_OVERRIDE(ArriveAction)
};

class ExitAction : public Action {
private:
	Outgoing* outgoing;
public:
	ExitAction() = delete;
	ExitAction(ShuntingUnit* su, int duration, Outgoing* outgoing) : Action(su, duration), outgoing(outgoing) {};
	ExitAction(ShuntingUnit* su, Outgoing* outgoing) : Action(su), outgoing(outgoing) {};
	inline Track* GetDestinationTrack() const { return outgoing->GetParkingTrack(); }
	inline const vector<Track*> GetReservedTracks() const override { return (outgoing->IsInstanding() ? vector<Track*>() : vector<Track*>({ GetDestinationTrack() })); }
	inline Outgoing* GetOutgoing() const { return outgoing; }
	ACTION_OVERRIDE(ExitAction)
};

class BeginMoveAction : public Action {
public:
	BeginMoveAction() = delete;
	BeginMoveAction(ShuntingUnit* su, int duration) : Action(su, duration) {};
	ACTION_OVERRIDE(BeginMoveAction)
};

class EndMoveAction : public Action {
public:
	EndMoveAction() = delete;
	EndMoveAction(ShuntingUnit* su, int duration) : Action(su, duration) {};
	ACTION_OVERRIDE(EndMoveAction)
};

class MoveAction : public Action {
private:
	const vector<Track*> tracks;
public:
	MoveAction() = delete;
	MoveAction(ShuntingUnit* su, const vector<Track*> &tracks, int duration) : Action(su, duration), tracks(tracks) {};
	MoveAction(ShuntingUnit* su, const vector<Track*> &tracks) : Action(su), tracks(tracks) {};
	inline Track* GetDestinationTrack() const { return tracks.back(); }
	inline Track* GetPreviousTrack() const { return tracks[tracks.size()-2]; }
	inline const vector<Track*>& GetTracks() const { return tracks; }
	inline const vector<Track*> GetReservedTracks() const override { return tracks; }
	ACTION_OVERRIDE(MoveAction)
};

class CombineAction : public Action {};
class SplitAction : public Action {};

class ServiceAction : public Action {
private:
	Train* train;
	Facility* facility;
	Task* task;
	vector<Employee*> employees;
public:
	ServiceAction() = delete;
	ServiceAction(ShuntingUnit* su, Train* tu, Task* ta, Facility* fa, const vector<Employee*>& ems) :
		Action(su, ta->duration), train(tu), task(ta), facility(fa), employees(ems) {}
	inline Train* GetTrain() const { return train; }
	inline Facility* GetFacility() const { return facility; }
	inline Task* GetTask() const { return task; }
	const inline vector<Employee*>& GetEmployees() const { return employees; }
	ACTION_OVERRIDE(ServiceAction)
};

class SetbackAction : public Action {
private:
	vector<Employee*> drivers;
public:
	SetbackAction() = delete;
	SetbackAction(ShuntingUnit* su, vector<Employee*> drivers, int duration) : Action(su, duration), drivers(drivers) {}
	const inline vector<Employee*>& GetDrivers() const { return drivers; }
	ACTION_OVERRIDE(SetbackAction)
};

class WaitAction : public Action {
public:
	WaitAction() = delete;
	WaitAction(ShuntingUnit* su, int duration) : Action(su, duration) {}
	ACTION_OVERRIDE(WaitAction)
};

class ActionValidator {
private:
	vector<BusinessRule*> validators;
	const Config* config;
	void AddValidators();
public:
	ActionValidator(const Config* config) : config(config) {
		AddValidators();
	}
	~ActionValidator();
	pair<bool, string> IsValid(State* state, Action* action) const;
	void FilterValid(State* state, list<Action*>& actions) const;
};

class ActionGenerator {
public:
	ActionGenerator() = delete;
	ActionGenerator(const ActionGenerator& am) = delete;
	ActionGenerator(const json& params) {}
	~ActionGenerator() = default;
	virtual void Generate(State* state, list<Action*>& out) const = 0;
};

class ActionManager {
private:
	vector<ActionGenerator*> generators;
	const Config* config;
	void AddGenerators();
	void AddGenerator(string name, ActionGenerator* generator);
public:
	ActionManager() = delete;
	ActionManager(const ActionManager& am) = delete;
	ActionManager(const Config* config) : config(config) {
		AddGenerators();
	}
	~ActionManager();
	void Generate(State* state, list<Action*>& out) const;

};

#ifndef OVERRIDE_ACTIONGENERATOR
#define OVERRIDE_ACTIONGENERATOR(name) \
	name() = delete; \
	name(const name& n) = delete; \
	void Generate(State* state, list<Action*>& out) const override;
#endif

#ifndef DEFINE_ACTIONGENERATOR
#define DEFINE_ACTIONGENERATOR(name) \
class name : public ActionGenerator { \
public: \
	name(const json& params) : ActionGenerator(params) {}; \
	OVERRIDE_ACTIONGENERATOR(name) \
};
#endif

DEFINE_ACTIONGENERATOR(ArriveActionGenerator)
DEFINE_ACTIONGENERATOR(ExitActionGenerator)
DEFINE_ACTIONGENERATOR(BeginMoveActionGenerator)
DEFINE_ACTIONGENERATOR(EndMoveActionGenerator)
DEFINE_ACTIONGENERATOR(WaitActionGenerator)
DEFINE_ACTIONGENERATOR(ServiceActionGenerator)

class MoveActionGenerator : public ActionGenerator {
private:
	int noRoutingDuration, constantTime;
	bool defaultTime, normTime, walkTime;
public:
	MoveActionGenerator(const json& params);
	OVERRIDE_ACTIONGENERATOR(MoveActionGenerator)
};

class SetbackActionGenerator : public ActionGenerator {
private:
	bool defaultTime, normTime, walkTime;
	int constantTime;
	int GetDuration(State* state, ShuntingUnit* su, int numDrivers) const;
public:
	SetbackActionGenerator(const json& params);
	OVERRIDE_ACTIONGENERATOR(SetbackActionGenerator)
};



