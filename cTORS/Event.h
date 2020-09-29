#pragma once
#include "Action.h"

enum class EventType { ActionFinish, IncomingTrain, OutgoingTrain, DisturbanceBegin, DisturbanceEnd, Trigger, MoveUpdate };

class Event
{
private:
	int time;
	Action* action;
	EventType type;
public:
	Event(int time, Action* action, EventType eventType);
	Event(const Event &e);
	~Event();
	inline int GetTime() { return time; }
	inline Action* GetAction() { return action; }
	inline EventType GetType() { return type; }
	inline friend bool operator< (const Event& lhs, const Event& rhs) {
		return lhs.time < rhs.time;
	}
	inline friend bool operator> (const Event& lhs, const Event& rhs) {
		return lhs.time > rhs.time;
	}
	inline string toString() const {
		switch (type) {
		case EventType::ActionFinish: return "Finish action " + (action == nullptr ? "X" : action->toString());
		case EventType::IncomingTrain: return "Incoming train";
		case EventType::OutgoingTrain: return "Outgoing train";
		default: return "Event";
		};
	}
};

