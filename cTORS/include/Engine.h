/** \file Engine.h
 * Describes the Engine and the LocationEngine classes
 */
#pragma once
#ifndef ENGINE_H
#define ENGINE_H
#include "State.h"
#include "Plan.h"

using namespace std;

class RunResult;
class POSPlan;

/**
 * A TORS engine for a specific Location
 */
class LocationEngine
{
private:
	string path;
	Location location;
	Scenario originalScenario;
	Config config;
	ActionValidator actionValidator;
	ActionManager actionManager;
	unordered_map<State*, list<const Action*>> stateActionMap;
	unordered_map<State*, RunResult*> results;

	void ExecuteEvent(State* state, const Event* e);
	void ExecuteImmediateEvents(State * state);
public:
	LocationEngine() = delete;
	/** Construct a LocationEngine based on the configuration files found in the given folder */
	LocationEngine(const string &path);
	/** Destroy this LocationEngine */
	~LocationEngine();
	/** Get a list of valid Action%s for the given State */
	list<const Action*> &GetValidActions(State* state);
	/** Go to the next Step in the simulation and update the State */
	list<const Action*> &Step(State* state, Scenario* scenario = nullptr);
	/** Apply the given Action to the State */
	void ApplyAction(State* state, const Action* action);
	/** Apply the given SimpleAction to the State */
	void ApplyAction(State* state, const SimpleAction& action);
	/** Generate an Action from the given SimpleAction */
	const Action* GenerateAction(const State* state, const SimpleAction& action) const;
	/** Evaluate the given POSPlan for the given Scenario */
	bool EvaluatePlan(const Scenario& scenario, const POSPlan& plan);
	/** Start a session for the given Scenario and generate an initial State */
	State* StartSession(const Scenario& scenario);
	/** Start a session for the original Scenario and generate an initial State */
	inline State* StartSession() { return StartSession(originalScenario); }
	/** End the session that belongs to the given State */
	void EndSession(State* state);
	/** Get a reference to the Location of this Engine */
	inline const Location& GetLocation() const { return location; }
	/** Get the original Scenario for this Location (to be updated) */
	inline const Scenario& GetScenario() const { return originalScenario; }
	
	/** Calculate all the shortest paths (run this once before requesting shortest paths) */ 
	void CalcShortestPaths();
	/** Get a path for the Move */
	const Path GetPath(const State* state, const Move& move) const;
	/** Get the RunResult for the given State/session */
	RunResult* GetResult(State* state) const { return results.at(state); }
	/** Import a RunResult from a protobuf file */
	RunResult* ImportResult(const string& path);
};

/**
 * The TORS Engine for several Location%s
 */
class Engine
{
private:
	map<const string, LocationEngine> engines;
	map<const State*, LocationEngine*> engineMap;
public:
	/** The default constructor */
	Engine() = default;
	/** Start the Engine with one Location as described by the given folder */
	Engine(const string& path);
	/** Engine desetructor */
	~Engine() = default;
	/** Get or load the LocationEngine based on its file location */
	LocationEngine* GetOrLoadLocationEngine(const string& location);
	/** Get the valid actions for the session with the given State */
	inline list<const Action*> &GetValidActions(State* state) const { return engineMap.at(state)->GetValidActions(state); }
	/** Go to the next Step in the simulation and update the State */
	inline list<const Action*> &Step(State* state, Scenario* scenario = nullptr) const { return engineMap.at(state)->Step(state, scenario); }
	/** Apply the given Action to the State */
	inline void ApplyAction(State* state, const Action* action) const { engineMap.at(state)->ApplyAction(state, action); }
	/** Apply the given SimpleAction to the State */
	inline void ApplyAction(State* state, const SimpleAction& action) const {engineMap.at(state)->ApplyAction(state, action); }
	/** Generate an Action from the given SimpleAction */
	inline const Action* GenerateAction(const State* state, const SimpleAction& action) const { return engineMap.at(state)->GenerateAction(state, action); }
	/** Evaluate the given POSPlan for the given Scenario on the given Location */
	inline bool EvaluatePlan(const string& location, const Scenario& scenario, const POSPlan& plan) { 
		return GetOrLoadLocationEngine(location)->EvaluatePlan(scenario, plan); }
	/** Start a session for the given Scenario and location and generate an initial State */
	State* StartSession(const string& location, const Scenario& scenario);
	/** Start a session for the original Scenario of a location and generate an initial State */
	State* StartSession(const string& location );
	/** End the session that belongs to the given State */
	void EndSession(State* state);
	
	/** Get a reference to the Location of the given location string */
	inline const Location& GetLocation(const string& location) const { return engines.at(location).GetLocation(); }
	/** Get a reference to the origial Scenario of the given location string (to be updated) */
	inline const Scenario& GetScenario(const string& location) const { return engines.at(location).GetScenario(); }
	/** Calculate all the shortest paths (run this once before requesting shortest paths) */ 
	void CalcShortestPaths();
	/** Get a path for the Move */
	inline const Path GetPath(const State* state, const Move& move) const { return engineMap.at(state)->GetPath(state, move); }
	/** Get the RunResult for the given State/session */
	inline RunResult* GetResult(State* state) const { return engineMap.at(state)->GetResult(state); }
	/** Import a RunResult from a protobuf file */
	RunResult* ImportResult(const string& path);
};

#endif