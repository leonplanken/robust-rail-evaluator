#include "State.h"

State::State(const Scenario& scenario) {
	time = scenario.GetStartTime();
	startTime = scenario.GetStartTime();
	endTime = scenario.GetEndTime();
	incomingTrains = scenario.GetIncomingTrains();
	outgoingTrains = scenario.GetOutgoingTrains();
	employees = scenario.GetEmployees();
	for (auto in : incomingTrains)
		AddEvent(in);
	for (auto out : outgoingTrains)
		AddEvent(out);
	changed = true;
}

State::~State() {
	shuntingUnits.clear();
	for (auto it : activeActions) {
		it.second.clear();
	}
}

void State::SetTime(int time) {
	if (time != this->time) {
		changed = true;
		this->time = time;
	}
}

Event* State::PeekEvent() const
{
	if (events.size() == 0)
		return nullptr;
	return events.top();
}

Event* State::PopEvent()
{
	Event* evnt = events.top();
	events.pop();
	return evnt;
}

void State::AddEvent(Incoming *in) {
	Event* event = new Event(in->GetTime(), nullptr, EventType::IncomingTrain);
	events.push(event);
	debug_out("Push event " << event->toString() << " at T=" << to_string(event->GetTime()));
}

void State::AddEvent(Outgoing *out) {
	Event* event = new Event(out->GetTime(), nullptr, EventType::OutgoingTrain);
	events.push(event);
	debug_out("Push event " << event->toString() << " at T=" << to_string(event->GetTime()));
}

void State::AddEvent(Action* action) {
	Event* event = new Event(time + action->GetDuration(), action, EventType::ActionFinish);
	events.push(event);
	debug_out("Push event " << event->toString() << " at T=" << to_string(event->GetTime()));
}

void State::StartAction(Action* action) {
	if (action == nullptr) return;
	changed = true;
	action->Start(this);
	AddEvent(action);
}

void State::FinishAction(Action* action) {
	if (action == nullptr) return;
	changed = true;
	action->Finish(this);
}

void State::MoveShuntingUnit(ShuntingUnit* su, Track* to, Track* previous) {
	RemoveOccupation(su);
	OccupyTrack(su, to, previous);
}

void State::RemoveOccupation(ShuntingUnit* su) {
	Track* current = GetPosition(su);
	auto& occ = occupations[current];
	auto it = find(occ.begin(), occ.end(), su);
	if (it != occ.end()) {
		occ.remove(*it);
	}
}

void State::OccupyTrack(ShuntingUnit* su, Track* track, Track* previous) {
	if(track->IsASide(previous))
		occupations[track].push_front(su);
	else
		occupations[track].push_back(su);
	SetPosition(su, track);
	SetPrevious(su, previous);
}

void State::ReserveTracks(const vector<Track*>& tracks) {
	for (Track* t : tracks)
		ReserveTrack(t);
}

void State::ReserveTracks(const list<Track*>& tracks) {
	for (Track* t : tracks)
		ReserveTrack(t);
}

void State::FreeTracks(const vector<Track*>& tracks) {
	for (Track* t : tracks)
		FreeTrack(t);
}

void State::FreeTracks(const list<Track*>& tracks) {
	for (Track* t : tracks)
		FreeTrack(t);
}

void State::RemoveIncoming(Incoming* incoming) {
	auto it = find(incomingTrains.begin(), incomingTrains.end(), incoming);
	if(it != incomingTrains.end())
		incomingTrains.erase(it);
}

void State::RemoveOutgoing(Outgoing* outgoing) {
	auto it = find(outgoingTrains.begin(), outgoingTrains.end(), outgoing);
	if (it != outgoingTrains.end())
		outgoingTrains.erase(it);
}

void State::RemoveShuntingUnit(ShuntingUnit* su) {
	RemoveOccupation(su);
	positions.erase(su);
	previous.erase(su);
	DELETE_LIST(activeActions[su]);
	activeActions.erase(su);
	moving.erase(su);
	waiting.erase(su);
	auto it = find(shuntingUnits.begin(), shuntingUnits.end(), su);
	if (it != shuntingUnits.end())
		shuntingUnits.erase(it);
}

const bool State::IsActive() {
	for (auto& su : shuntingUnits) {
		if (HasActiveAction(su) || IsWaiting(su)) return true;
	}
	return false;
}

const bool State::IsAnyInactive() {
	if (shuntingUnits.size() == 0) return false;
	for (auto& su : shuntingUnits) {
		if (!HasActiveAction(su) && !IsWaiting(su)) return true;
	}
	return false;
}

void State::RemoveActiveAction(ShuntingUnit* su, const Action* action) {
	auto& lst = activeActions.at(su);
	auto it = find_if(lst.begin(), lst.end(), [action](Action* a) -> bool { return *a == *action; } );
	if (it != lst.end()) {
		delete* it;
		lst.remove(*it);
	}
}

void State::addTasksToTrains(const map<Train*, vector<Task>>& tasks) {
	for (auto& it : tasks) {
		for (auto& task : it.second) {
			AddTaskToTrain(it.first, task);
		}
	}
}

void State::RemoveTaskFromTrain(Train* tu, const Task& task) {
	auto& lst = tasks.at(tu);
	auto it = find(lst.begin(), lst.end(), task);
	if (it != lst.end())
		lst.erase(it);
}

void State::RemoveActiveTaskFromTrain(Train* tu, Task* task) {
	auto& lst = activeTasks.at(tu);
	auto it = find(lst.begin(), lst.end(), task);
	if (it != lst.end())
		lst.erase(it);
}

int State::GetPositionOnTrack(ShuntingUnit* su) {
	auto sus = GetOccupations(GetPosition(su));
	auto it = find(sus.begin(), sus.end(), su);
	return static_cast<int>(distance(sus.begin(), it));
}

bool State::CanMoveToSide(ShuntingUnit* su, Track* side) {
	auto park = GetPosition(su);
	auto sus = GetOccupations(park);
	if (park->IsASide(side))
		return sus.front() == su;
	return sus.back() == su;
}

vector<Train*> State::GetTrainUnitsInOrder(ShuntingUnit* su) {
	auto trains = su->GetTrains();
	auto previous = GetPrevious(su);
	auto current = GetPosition(su);
	if (previous == nullptr || current->IsASide(previous))
		return trains;
	vector<Train*>reverse (trains.rbegin(), trains.rend());
	return reverse;
}

int State::GetDirection(ShuntingUnit* su) {
	auto current = GetPosition(su);
	auto previous = GetPrevious(su);
	if (previous == nullptr) return 0;
	else if (current->IsASide(previous)) return 1;
	return -1;
}