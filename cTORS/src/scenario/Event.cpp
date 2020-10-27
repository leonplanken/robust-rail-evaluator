#include "Event.h"

Event::Event(int time, Action* action, EventType eventType)
{
	this->time = time;
	if (action != nullptr)
		this->action = action->clone();
	else this->action = nullptr;
	this->type = eventType;
}

Event::Event(const Event &e) {
	time = e.time;
	if (e.action != nullptr)
		action = e.action->clone();
	else action = nullptr;
	type = e.type;
}

Event::~Event()
{
	if(action != nullptr)
		delete action;
}
