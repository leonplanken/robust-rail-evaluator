#pragma once
#include <vector>
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "Exceptions.h"

using json = nlohmann::json;
using namespace std;

//enum class Direction { A, B };

enum class TrackPartType { 
	Railroad,			/* A piece of track with one piece of track on both ends. */
	Switch,				/* A piece of track with one piece of track on one end and two pieces of
						track on the other end */
	EnglishSwitch,		/* A piece of track with two pieces of track on both ends. Here, both 
						a_sides tracks are connected to both b_sides tracks, but not a_side to 
						a_side or b_side to b_side. */
	HalfEnglishSwitch,	/* A piece of track with two pieces of track on both ends.Here, 
						a_sides[0] is connected to both b_sides tracks, while a_sides[1] is only 
						connected to b_sides[1]. */
	InterSection,		/* A piece of track with two pieces of track on both ends. Here, 
						a_sides[0] connects to b_sides[1] and a_sides[1] connects to b_sides[0]. */
	Bumper,				/* A piece of track which stops at some point. Here, a_side contains the 
						piece of track and b_side does not exist, as the track doesn't continue. */
	Building			/* A Railroad with a building on/over it */
};

NLOHMANN_JSON_SERIALIZE_ENUM(TrackPartType, {
	{TrackPartType::Railroad, "Railroad"},
	{TrackPartType::Switch, "Switch"},
	{TrackPartType::EnglishSwitch, "EnglishSwitch"},
	{TrackPartType::HalfEnglishSwitch, "HalfEnglishSwitch"},
	{TrackPartType::InterSection, "InterSection"},
	{TrackPartType::Bumper, "Bumper"},
	{TrackPartType::Building, "Building"},
	})

class Facility;

class Track {
private:
	vector<Track*> aSides;
	vector<Track*> bSides;
	map<Track*, vector<Track*>> next;
	vector<Facility*> facilities;
public:
	string id;
	TrackPartType type;
	double length;
	string name;
	bool sawMovementAllowed;
	bool parkingAllowed;
	bool isElectrified;
	bool standingAllowed;

	Track();
	Track(const string& id, TrackPartType type, double length, const string& name, bool sawMovementAllowed,
		bool parkingAllowed, bool isElectrified, bool standingAllowed);
	Track(const Track& track);
	~Track();
	void AssignNeighbors(vector<Track*> aside, vector<Track*> bside);
	
	bool IsASide(Track* t) const;
	bool IsBSide(Track* t) const;
	inline TrackPartType GetType() const { return type; }
	
	inline void AddFacility(Facility* f) {
		facilities.push_back(f);
	}

	inline const vector<Facility*> &GetFacilities() {
		return facilities;
	}
	
	inline const vector<Track*> &GetNextTrackParts(Track* previous) const {
		return next.at(previous);
	}

	inline const vector<Track*> GetNeighbors() const {
		vector<Track*> res = aSides;
		if(bSides.size() > 0)
			res.insert(res.end(), bSides.begin(), bSides.end());
		return res;
	}

	inline string toString() const { 
		return name;
	}
	
	inline bool operator==(const Track& t) const { return (id == t.id); }
	inline bool operator!=(const Track& t) const { return !(*this == t); }
};

void from_json(const json& j, Track& p);
