#include "TrainGoals.h"

const Train* GetTrainById(const vector<const Train*>& trains, const string& id) {
	int iid = id == "****" ? -1 : stoi(id);
	for(auto& train: trains) {
		if(train->GetID() == iid) return train;
	}
	return nullptr;
}

unordered_map<const Train*, vector<Task>, TrainHash, TrainEquals> ConvertPBTrainTasks(const ShuntingUnit* su, const PBTrainGoal& pb_inc) {
	unordered_map<const Train*, vector<Task>, TrainHash, TrainEquals> map;
	for(auto& train: pb_inc.members()) {
		for(auto& t: train.tasks()) {
			auto tu = GetTrainById(su->GetTrains(), train.id());
			map[tu].push_back({t});
		}
	}
	return map;
}

unordered_map<const Train*, vector<Task>, TrainHash, TrainEquals> ConvertPBTrainTasks(const ShuntingUnit* su, const PBIncoming& pb_inc) {
	unordered_map<const Train*, vector<Task>, TrainHash, TrainEquals> map;
	for(auto& train: pb_inc.shuntingunit().trainunits()) {
		for(auto& t: train.tasks()) {
			auto tu = GetTrainById(su->GetTrains(), train.id());
			map[tu].push_back({t});
		}
	}
	return map;
}

Incoming::Incoming(const PBIncoming& pb_inc, bool isInstanding) : Incoming(pb_inc.id(), new ShuntingUnit(pb_inc.shuntingunit()),
 	pb_inc.time(), isInstanding, pb_inc.standingindex()) {
		tasks = ConvertPBTrainTasks(shuntingUnit, pb_inc);
	 }

Incoming::Incoming(const PBTrainGoal& pb_inc, bool isInstanding) : Incoming(stoi(pb_inc.id()), new ShuntingUnit(pb_inc),
 	pb_inc.time(), isInstanding, pb_inc.standingindex()) {
		 tasks = ConvertPBTrainTasks(shuntingUnit, pb_inc);
	 }

Outgoing::Outgoing(const PBTrainGoal& pb_out, bool isInstanding) : Outgoing(stoi(pb_out.id()), new ShuntingUnit(pb_out),
 	pb_out.time(), isInstanding, pb_out.standingindex()) {}

TrainGoal::TrainGoal(const TrainGoal& traingoal) :
	id(traingoal.id), parkingTrack(traingoal.parkingTrack), sideTrack(traingoal.sideTrack),
	time(traingoal.time), standingIndex(traingoal.standingIndex), isInstanding(traingoal.isInstanding)
{
	shuntingUnit = new ShuntingUnit(*traingoal.shuntingUnit);
}

TrainGoal::~TrainGoal() {
	delete shuntingUnit;
}

void TrainGoal::assignTracks(const Track* park, const Track* side) {
	parkingTrack = park;
	sideTrack = side;
}