#pragma once
#include <vector>
#include <fstream>
#include <exception>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "Utils.h"
#include "Track.h"
#include "Facility.h"
#include "Exceptions.h"
#include "Utils.h"
using json = nlohmann::json;
using namespace std;

class Location
{
private:
	static const string locationFileString;
	
	vector<Track*> tracks;
	vector<Facility*> facilities;
	map<pair<const Track*, const Track*>, double> distanceMatrix;
	map<string, Track*> trackIndex;
	int movementConstant;
	map<const TrackPartType, int> moveDuration;
	
	void importTracksFromJSON(const json& j);
	void importFacilitiesFromJSON(const json& j);
	void importDistanceMatrix(const json& j);
public:
	Location() = delete;
	Location(const string &path);
	Location(const Location& location) = default;
	~Location();
	
	inline Track* getTrackByID(const string& id) const {
		return trackIndex.at(id);
	}

	inline const vector<Track*>& GetTracks() const { return tracks; }

	inline double GetDistance(const Track* from, const Track* to) const { return distanceMatrix.at({from, to}); }
	inline int GetDurationByType(const Track* track) const { 
		return (track->GetType() == TrackPartType::Railroad && track->GetLength() == 0) ? 0  : moveDuration.at(track->GetType()); }
};

