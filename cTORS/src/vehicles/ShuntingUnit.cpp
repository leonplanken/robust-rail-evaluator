#include "ShuntingUnit.h"

ShuntingUnit::ShuntingUnit(int id, vector<const Train*> trains) : id(id), trains(trains)
{
	UpdateValues();
}

template<class PBT>
vector<const Train*> ConvertPBTrains(const PBList<PBT> trains) {
	vector<const Train*> out;
	for(auto& train: trains) {
		out.push_back(new Train(train));
	}
	return out;
}

ShuntingUnit::ShuntingUnit(const PBTrainGoal& pb_tg) : ShuntingUnit(stoi(pb_tg.id()), ConvertPBTrains(pb_tg.members())) {}

void ShuntingUnit::UpdateValues() {
	length = 0;
	needsElectricity = false;
	for (auto& t : trains) {
		length += t->GetType()->length;
		needsElectricity |= t->GetType()->needsElectricity;
	}
}

ShuntingUnit::~ShuntingUnit() {
	DELETE_VECTOR(trains)
}

const string ShuntingUnit::GetTrainString() const {
	string result = "";
	bool first = true;
	for(auto t: GetTrains()) {
		result += (t->toString() + (first ? "" : "-"));
		first = false;
	}
	return result;
}

const Train* ShuntingUnit::GetTrainByID(int id) const {
	auto it = find_if(trains.begin(), trains.end(), [id](const Train* t) -> bool { return t->GetID() == id; });
	if(it == trains.end()) return nullptr;
	return *it;
}

int ShuntingUnit::GetTrainIndexByID(int id) const {
	auto it = find_if(trains.begin(), trains.end(), [id](const Train* t) -> bool { return t->GetID() == id; });
	if(it == trains.end()) return -1;
	return it - trains.begin();
}

ShuntingUnit::ShuntingUnit(const ShuntingUnit& su) :
	id(su.id), length(su.length) {
	for (auto t : su.trains)
		trains.push_back(new Train(*t));
	UpdateValues();
}

int ShuntingUnit::GetSetbackTime(const Train* const frontTrain, bool normTime, bool walkTime, int setbackTime) const {
	if(normTime && trains.size() > 0) {
		setbackTime += frontTrain->GetType()->backNormTime;
	}
	if(walkTime) {
		for(auto t: trains) {
			setbackTime += t->GetType()->setbackTime;
		}
	}
	return setbackTime;
		
}

bool ShuntingUnit::MatchesShuntingUnit(const ShuntingUnit* su) const {
	auto& su1Trains = GetTrains();
	auto& su2Trains = su->GetTrains();
	if(su1Trains.size() != su2Trains.size()) return false;
	bool leftValid = true;
	bool rightValid = true;
	for (int i = 0; i < su1Trains.size(); i++) {
		auto exp = su1Trains.at(i);
		auto left = su2Trains.at(i);
		auto right = su2Trains.at(su2Trains.size() - 1 - i);
		bool haveIDleft = (exp->GetID() != -1 && left->GetID() != -1);
		bool haveIDright = (exp->GetID() != -1 && right->GetID() != -1);
		if ((haveIDleft && exp->GetID() != left->GetID()) ||
			(!haveIDleft && exp->GetType()->displayName != left->GetType()->displayName))
			leftValid = false;
		if ((haveIDright && exp->GetID() != right->GetID()) ||
			(!haveIDright && exp->GetType()->displayName != right->GetType()->displayName))
			rightValid = false;	
		if(!leftValid && !rightValid) return false;
	}
	return true;
}