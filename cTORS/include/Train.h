#pragma once
#include <string>
#include <list>
#include <vector>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

struct TrainUnitType {
	static map<string,TrainUnitType*> types;
	string displayName;
	int carriages;
	double length;
	int combineDuration;
	int splitDuration;
	int backNormTime;
	int backAdditionTime;
	int travelSpeed;
	int startUpTime;
	string typePrefix;
	bool needsLoco;
	bool isLoco;
	bool needsElectricity;

	TrainUnitType() = default;
	TrainUnitType(const string& displayName, int carriages, double length, int combineDuration, int splitDuration, int backNormTime,
		int backAdditionTime, int travelSpeed, int startUpTime, const string& typePrefix, bool needsLoco, bool isLoco, bool needsElectricity) :
		displayName(displayName), carriages(carriages), length(length), combineDuration(combineDuration), splitDuration(splitDuration),
		backNormTime(backNormTime), backAdditionTime(backAdditionTime), travelSpeed(travelSpeed), startUpTime(startUpTime), typePrefix(typePrefix),
		needsLoco(needsLoco), isLoco(isLoco), needsElectricity(needsElectricity) {}
	string toString() { return displayName; }
	bool operator==(const TrainUnitType& t) const { return (displayName == t.displayName); }
	bool operator!=(const TrainUnitType& t) const { return !(*this == t); }

	
};

struct Task {
	string taskType;
	int priority;
	int duration;
	list<string> skills;
	string toString() { return taskType; }
	
	Task() = default;
	Task(const string& taskType, int priority, int duration, list<string> skills) :
		taskType(taskType), priority(priority), duration(duration), skills(skills) {}
	bool operator==(const Task& t) const { return (taskType == t.taskType && priority == t.priority && duration == t.duration); }
	bool operator!=(const Task& t) const { return !(*this == t); }
};

class Train
{
private:
	int id;
	TrainUnitType *type;
	bool forcedMatch;
public:
	Train() = default;
	Train(int id, TrainUnitType *type);
	Train(const Train &train);
	~Train();
	void fromJSON(const json& j);
	inline TrainUnitType* GetType() const { return type; }
	bool operator==(const Train& train) const;
	bool operator!=(const Train& t) const { return !(*this == t); }
	string toString();
	inline int GetID() const { return id; }
	inline void SetID(int id) { this->id = id; }
};

inline void from_json(const json& j, Train& t) {
	t.fromJSON(j);
}

void from_json(const json& j, TrainUnitType& tt);

void from_json(const json& j, Task& t);