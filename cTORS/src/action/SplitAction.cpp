#include "Action.h"
#include "State.h"

void SplitAction::Start(State *state) const
{
	auto su = GetShuntingUnit();
	auto &suState = state->GetShuntingUnitState(su);
	auto track = suState.position;
	auto previous = suState.previous;
	int positionOnTrack = state->GetPositionOnTrack(su);
	bool front = *suState.frontTrain == su->GetTrains().front();
	bool inNeutral = suState.inNeutral;
	bool fromASide = track->IsASide(previous);

	// cout << " Test - SplitAction::Start(State *state) " << endl;
	
	auto _suA(suA);
	auto _suB(suB);

	// Added by R.G.Kromes 05/02/2025
	// Tasks per train units must be stored, and later assinged to the state state->AddTasksToTrains(...),
	// this step is needed because after calling state->RemoveShuntingUnit(su); the tasks per splited trains 
	// will be lost
	unordered_map<const Train*, vector<Task>, TrainHash, TrainEquals> tasks_per_train_A; 
	for (auto &tu : suA.GetTrains())
	{
		// Tasks per train ?
		// cout << "TrainUnit" << tu << endl;
		for (const Task &task : state->GetTasksForTrain(&tu))
		{
			cout << task << endl;
			tasks_per_train_A[&tu].push_back(task);
		}

	}
	// Added by R.G.Kromes 05/02/2025
	// Tasks per train units must be stored, and later assinged to the state state->AddTasksToTrains(...),
	// this step is needed because after calling state->RemoveShuntingUnit(su); the tasks per splited trains 
	// will be lost
	unordered_map<const Train*, vector<Task>, TrainHash, TrainEquals> tasks_per_train_B; 
	for (auto &tu : suB.GetTrains())
	{
		// Tasks per train ?
		// cout << "TrainUnit" << tu << endl;
		for (const Task &task : state->GetTasksForTrain(&tu))
		{
			cout << task << endl;
			tasks_per_train_B[&tu].push_back(task);
		}

	}

	state->RemoveShuntingUnit(su);


	state->AddShuntingUnitOnPosition(&_suA, track, previous, front ? &_suA.GetTrains().front() : &_suA.GetTrains().back(), positionOnTrack);
	state->AddShuntingUnitOnPosition(&_suB, track, previous, front ? &_suB.GetTrains().front() : &_suB.GetTrains().back(), positionOnTrack + (fromASide ? 0 : 1));	
	
	// Added by R.G.Kromes 05/02/2025
	// the tasks of the trains before the split action must be assigned
	// to the "new" state of the trains. Somehow, when "state->RemoveShuntingUnit(su)"; 
	// is called on the shunting unit that was not yet split into parts, the tasks of the
	// splitted trains will be lost
	state->AddTasksToTrains(tasks_per_train_A);
	state->AddTasksToTrains(tasks_per_train_B);

	state->SetInNeutral(&_suA, inNeutral);
	state->SetInNeutral(&_suB, inNeutral);

	state->AddActiveAction(&_suA, this);
	state->AddActiveAction(&_suB, this);

	/*
	Suppose a SU T1-T2-T3, with front train T1, and split-index 1
	resulting in suA: T1, with front train T1. suB: T2-T3, with front train T2
	The track setup is as follows: A < --- SU ( T3 - T2 - T1> ) ---- > B
	This means it previous track is at the Aside, but the front train is directed to the Bside.
	In this case front = true, fromASide = true
	The resulting setup must be: A < --- suB ( T3 - T2> ) - suA ( T1> ) ---- > B
	*/
	// state->AddShuntingUnitOnPosition(&suA, track, previous, front ? &suA.GetTrains().front() : &suA.GetTrains().back(), positionOnTrack);
	// state->AddShuntingUnitOnPosition(&suB, track, previous, front ? &suB.GetTrains().front() : &suB.GetTrains().back(), positionOnTrack + (fromASide ? 0 : 1));	
	// state->SetInNeutral(&suA, inNeutral);
	// state->SetInNeutral(&suB, inNeutral);

	// state->AddActiveAction(&suA, this);
	// state->AddActiveAction(&suB, this);
}

void SplitAction::Finish(State *state) const
{
	state->RemoveActiveAction(&suA, this);
	state->RemoveActiveAction(&suB, this);
}

const string SplitAction::toString() const
{
	return "Split " + suString + " into " + suA.GetTrainString() + " and " + suB.GetTrainString();
}

// TODOs somehow the taks after a split are lost ????
const Action *SplitActionGenerator::Generate(const State *state, const SimpleAction &action) const
{
	auto split = static_cast<const Split *>(&action);
	auto su = InitialCheck(state, action);
	auto &suState = state->GetShuntingUnitState(su);
	if (su->GetTrains().size() <= 1)
		throw InvalidActionException("The shunting unit consists of only one train unit.");
	if (suState.moving || suState.waiting)
		throw InvalidActionException("The shunting unit is already occupied.");
	auto duration = suState.frontTrain->GetType()->splitDuration;
	auto splitPosition = split->GetSplitIndex();
	if (splitPosition < 1 || splitPosition >= su->GetTrains().size())
		throw InvalidActionException("The split index is invalid.");
	const vector<Train> trains = state->GetTrainUnitsInOrder(su);
	ShuntingUnit suA = ShuntingUnit(su->GetID(), vector<Train>(trains.begin(), trains.begin() + splitPosition));
	int suBid = su->GetID() + 1;
	// cout << ">>>>>> From SplitActionGenerator " << endl;
	// for(auto &tu : suA.GetTrains())
	// {
	// 	// Tasks per train ?
	// 	cout << "TrainUnit" << tu << endl;
	// 	for(const Task &task : state->GetTasksForTrain(&tu))
	// 	{
	// 		cout << task << endl;
	// 	}
	// }

	while (state->HasShuntingUnitByID(suBid))
		suBid++;
	ShuntingUnit suB = ShuntingUnit(suBid, vector<Train>(trains.begin() + splitPosition, trains.end()));
	return new SplitAction(su, suState.position, duration, suA, suB);
}

void SplitActionGenerator::Generate(const State *state, list<const Action *> &out) const
{
	if (state->GetTime() == state->GetEndTime())
		return;
	// TODO employees
	for (const auto &[su, suState] : state->GetShuntingUnitStates())
	{
		auto size = su->GetTrains().size();
		if (size <= 1 || suState.moving || suState.waiting || suState.HasActiveAction())
			continue;
		auto duration = suState.frontTrain->GetType()->splitDuration;
		for (int splitPosition = 1; splitPosition < size; splitPosition++)
		{
			out.push_back(Generate(state, Split(su, splitPosition)));
		}
	}
}