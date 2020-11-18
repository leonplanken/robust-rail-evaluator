#include "Location.h"

using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace std;


const string Location::locationFileString = "location.json";

Location::Location(const string &folderName) {
	try {
		ifstream fileInput(fs::path(folderName) / fs::path(locationFileString));
		if (!fileInput.good())
			throw InvalidLocationException("The specified file '" + folderName + "' does not exist");
		json j;
		fileInput >> j;
		importTracksFromJSON(j["trackParts"]);
		importFacilitiesFromJSON(j["facilities"]);
		importDistanceMatrix(j["distanceEntries"]);
		j["movementConstant"].get_to(movementConstant);
		int movementTrackCoefficient = j["movementTrackCoefficient"].get<int>();
		int movementSwitchCoefficient = j["movementSwitchCoefficient"].get<int>();
		moveDuration[TrackPartType::Railroad] = movementTrackCoefficient;
		moveDuration[TrackPartType::Switch] = movementSwitchCoefficient;
		moveDuration[TrackPartType::EnglishSwitch] = 2 * movementSwitchCoefficient;
		moveDuration[TrackPartType::HalfEnglishSwitch] = 2 * movementSwitchCoefficient;
		moveDuration[TrackPartType::InterSection] = 0;
		moveDuration[TrackPartType::Bumper] = 0;
	}
	catch (exception& e) {
		cout << "Error in loading location: " << e.what() << "\n";
		throw e;
	}
}

Location::~Location()
{
	DELETE_VECTOR(tracks)
	DELETE_VECTOR(facilities)
	trackIndex.clear();
}

void Location::importTracksFromJSON(const json& j) {
	map<Track*, vector<string>> aSides;
	map<Track*, vector<string>> bSides;
	for (auto& jit : j) {
		Track* t = new Track();
		jit.get_to(*t);
		aSides[t] = jit["aSide"].get<vector<string>>();
		bSides[t] = jit["bSide"].get<vector<string>>();
		tracks.push_back(t);
		trackIndex[t->id] = t;
		debug_out("Imported track " << t->toString());
	}
	for (Track* t : tracks) {
		vector<string> saside = aSides[t];
		vector<string> sbside = bSides[t];
		auto aside = vector<const Track*>(saside.size());
		auto bside = vector<const Track*>(sbside.size());
		for (int j = 0; j != saside.size(); j++)
			aside[j] = trackIndex[saside[j]];
		for (int j = 0; j != sbside.size(); j++)
			bside[j] = trackIndex[sbside[j]];
		t->AssignNeighbors(aside, bside);
	}
	debug_out("finished loading tracks from JSON");
}

void Location::importFacilitiesFromJSON(const json& j) {
	for (auto& jit : j) {
		Facility* f = new Facility();
		jit.get_to(*f);
		auto sTracks = jit["relatedTrackParts"].get<vector<string>>();
		vector<Track*> tracks;
		for (string s : sTracks) {
			Track* t = trackIndex[s];
			tracks.push_back(t);
			t->AddFacility(f);
		}
		f->AssignTracks(tracks);
		facilities.push_back(f);
		debug_out("Imported facility " << f->toString());
	}
	debug_out("finished loading facilities from JSON");
}

void Location::importDistanceMatrix(const json& j) {
	for (auto& jit : j) {
		string from = jit.at("fromTrackPartId").get<string>();
		string to = jit.at("toTrackPartId").get<string>();
		int distance = jit.at("distanceInSeconds").get<int>();
		pair<Track*, Track*> key(trackIndex[from], trackIndex[to]);
		distanceMatrix[key] = distance;
	}
}
