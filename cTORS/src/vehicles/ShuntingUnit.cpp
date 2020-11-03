#include "ShuntingUnit.h"

ShuntingUnit::ShuntingUnit(int id, vector<Train*> trains) : id(id), trains(trains)
{
	UpdateValues();
}

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

ShuntingUnit::ShuntingUnit(const ShuntingUnit& su) :
	id(su.id), length(su.length) {
	for (auto t : su.trains)
		trains.push_back(new Train(*t));
	UpdateValues();
}

void ShuntingUnit::fromJSON(const json& j) {
	id = stoi(j.at("id").get<string>());
	for (auto jit : j.at("members")) {
		Train* t = new Train();
		jit.get_to(*t);
		trains.push_back(t);
	}
	UpdateValues();
}
