#pragma once
#include <list>
#include <map>
#include "Utils.h"
#include "Event.h"
#include "Action.h"
#include "EventQueue.h"
#include "Scenario.h"
#include "TrainGoals.h"
using namespace std;

class State
{
private:
	EventQueue events;
	int time, startTime, endTime;
	vector<Incoming*> incomingTrains;
	vector<Outgoing*> outgoingTrains;
	vector<Employee*> employees;
	vector<ShuntingUnit*> shuntingUnits;
	map<ShuntingUnit*, Track*> positions;
	map<Track*, list<ShuntingUnit*>> occupations;
	map<ShuntingUnit*, Track*> previous;
	map<Track*, bool> reserved;
	map<ShuntingUnit*, list<Action*>> activeActions;
	map<ShuntingUnit*, bool> moving;
	map<ShuntingUnit*, bool> waiting;
	map<Train*, vector<Task>> tasks;
	map<Train*, list<Task*>> activeTasks;
	bool changed;
public:
	State() = delete;
	State(const Scenario& scenario);
	State(const State& state) = default;
	~State();

	//Events
	inline size_t GetNumberOfEvents() const { return events.size(); }
	Event* PeekEvent() const;
	Event* PopEvent();
	void AddEvent(Incoming* in);
	void AddEvent(Outgoing* out);
	void AddEvent(Action* action);

	//Time
	void SetTime(int time);
	inline int GetTime() const { return time; };
	inline int GetEndTime() const { return endTime; }
	inline int GetStartTime() const { return startTime; }
	
	//Changed
	inline bool IsChanged() const { return changed; }
	inline void SetUnchanged() { changed = false; }
	
	//Apply action
	void StartAction(Action* action);
	void FinishAction(Action* action);

	//Getters
	inline const vector<Incoming*>& GetIncomingTrains() const { return incomingTrains; }
	inline const vector<Outgoing*>& GetOutgoingTrains() const { return outgoingTrains; }
	inline Track* GetPosition(ShuntingUnit* su)  { return positions[su]; }
	inline Track* GetPrevious(ShuntingUnit* su) { return previous[su]; }
	inline const list<ShuntingUnit*>& GetOccupations(Track* track)  { return occupations[track]; }
	int GetPositionOnTrack(ShuntingUnit* su);
	inline size_t GetAmountOnTrack(Track* track) { return occupations[track].size(); }
	vector<Train*> GetTrainUnitsInOrder(ShuntingUnit* su);
	bool CanMoveToSide(ShuntingUnit* su, Track* side);
	inline bool IsMoving(ShuntingUnit* su) { return moving[su]; }
	inline bool IsReserved(Track* track) { return reserved[track]; }
	inline bool IsWaiting(ShuntingUnit* su) { return waiting[su]; }
	inline const vector<ShuntingUnit*> GetShuntingUnits() const { return shuntingUnits; }
	inline const bool HasActiveAction(ShuntingUnit* su) { return activeActions[su].size() > 0; }
	inline const list<Action*> &GetActiveActions(ShuntingUnit* su) { return activeActions[su]; }
	const bool IsActive();
	const bool IsAnyInactive();
	int GetDirection(ShuntingUnit* su);
	inline vector<Task>& GetTasksForTrain(Train* tu) { return tasks[tu]; }
	inline map<Train*, list<Task*>>& GetActiveTasks() { return activeTasks; }

	//Setters and Adders
	inline void SetMoving(ShuntingUnit* su, bool b) { moving[su] = b; }
	inline void SetWaiting(ShuntingUnit* su, bool b) { waiting[su] = b; }
	inline void SetPosition(ShuntingUnit* su, Track* track) { positions[su] = track; }
	inline void SetPrevious(ShuntingUnit* su, Track* track) { previous[su] = track; }

	void addTasksToTrains(const map<Train*, vector<Task>>& tasks);
	inline void AddTaskToTrain(Train* tu, const Task& task) { tasks[tu].push_back(task); }
	inline void AddActiveTaskToTrain(Train* tu, Task* task) { activeTasks[tu].push_back(task); }
	inline void AddActiveAction(ShuntingUnit* su, const Action* action) { activeActions[su].push_back(action->clone()); }
	inline void AddShuntingUnit(ShuntingUnit* su) { shuntingUnits.push_back(su); }
	
	//Track Reservation
	void ReserveTracks(const vector<Track*>& tracks);
	void ReserveTracks(const list<Track*>& tracks);
	inline void ReserveTrack(Track* track) { reserved[track] = true; };
	void FreeTracks(const vector<Track*>& tracks);
	void FreeTracks(const list<Track*>& tracks);
	inline void FreeTrack(Track* track) { reserved[track] = false; };

	//Moving
	void MoveShuntingUnit(ShuntingUnit* su, Track* to, Track* previous);
	void OccupyTrack(ShuntingUnit* su, Track* park, Track* previous);
	
	//Remove
	void RemoveIncoming(Incoming* incoming);
	void RemoveOutgoing(Outgoing* outgoing);
	void RemoveShuntingUnit(ShuntingUnit* su);
	void RemoveActiveAction(ShuntingUnit* su, const Action* action);
	void RemoveOccupation(ShuntingUnit* su);
	void RemoveTaskFromTrain(Train* tu, const Task& task);
	void RemoveActiveTaskFromTrain(Train* tu, Task* task);
};

