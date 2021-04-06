#include "Train.h"

map<string, TrainUnitType *> TrainUnitType::types;

Train::Train(int id, TrainUnitType* type) : id(id), type(type), forcedMatch(false) {}

Train::Train(const PBTrainUnit& pb_train) 
	: Train(pb_train.id()=="****" ? -1 : stoi(pb_train.id()), TrainUnitType::types.at(pb_train.typedisplayname())) {}

Train::Train(const PBSTrainUnit& pb_train) 
	: Train(pb_train.id()=="****" ? -1 : stoi(pb_train.id()), TrainUnitType::types.at(pb_train.typedisplayname())) {}

Train::Train(const Train &train) : id(train.id), type(train.type), forcedMatch(train.forcedMatch) {}

Train::~Train() {}

bool Train::operator==(const Train& train) const {
	return (id != -1 && id == train.id) || this == &train;
}

const string Train::toString() const {
	return "(" + to_string(id) + ", " + type->toString() + ")";
}
