#pragma once
#include <vector>
#include <nlohmann/json.hpp>
#include "Train.h"
#include "Track.h"
#include "Utils.h"
using namespace std;
using json = nlohmann::json;

class Action;

class ShuntingUnit {
private:
	int id;
	vector<Train*> trains;
	double length;
	bool needsElectricity;
	void UpdateValues();
public:
	ShuntingUnit() = default;
	ShuntingUnit(int id, vector<Train*> trains);
	ShuntingUnit(const ShuntingUnit& su);
	~ShuntingUnit();
	void fromJSON(const json& j);

	inline string toString() const {
		return "ShuntingUnit-" + to_string(id);
	}

	inline bool operator==(const ShuntingUnit& su) const { return (id == su.id); }
	inline bool operator!=(const ShuntingUnit& su) const { return !(*this == su); }

	inline double GetLength() const { return length; }
	inline bool NeedsElectricity() const { return needsElectricity; }
	inline size_t GetNumberOfTrains() const { return trains.size(); }
	inline const vector<Train*>& GetTrains() const { return trains; }
	inline void SetTrains(vector<Train*> trains) {
		this->trains = trains;
		UpdateValues();
	}
};

inline void from_json(const json& j, ShuntingUnit& su) {
	su.fromJSON(j);
}

