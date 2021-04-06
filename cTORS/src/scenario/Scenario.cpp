#include "Scenario.h"

const string Scenario::scenarioFileString = "scenario.json";

Scenario::Scenario() : startTime(0), endTime(0)
{
}

Scenario::Scenario(string folderName, const Location& location) {
	try {
		PBScenario pb_scenario;
		parse_json_to_pb(fs::path(folderName) / fs::path(scenarioFileString), &pb_scenario);
		ImportEmployees(pb_scenario, location);
		ImportShuntingUnits(pb_scenario, location);
		startTime = pb_scenario.starttime();
		endTime = pb_scenario.endtime();
	}
	catch (exception& e) {
		cout << "Error in loading scenario: " << e.what() << "\n";
		throw e;
	}
}

Scenario::Scenario(const Scenario& scenario) : 
	startTime(scenario.startTime), endTime(scenario.endTime)
{
	disturbances = scenario.disturbances;
	for (auto inc : scenario.incomingTrains)
		incomingTrains.push_back(new Incoming(*inc));
	for (auto out : scenario.outgoingTrains)
		outgoingTrains.push_back(new Outgoing(*out));
	for (auto e : scenario.employees)
		employees.push_back(new Employee(*e));
	//TODO disturbances
	//TODO tasks
}


Scenario::~Scenario()
{
	DELETE_VECTOR(incomingTrains)
	DELETE_VECTOR(outgoingTrains)
	DELETE_VECTOR(employees)
	//TODO  disturbances
	//TODO tasks
}

const size_t Scenario::GetNumberOfTrains() const {
	size_t res = 0;
	for (auto inc : incomingTrains)
		res += inc->GetShuntingUnit()->GetNumberOfTrains();
	return res;
}

void Scenario::ImportEmployees(const PBScenario& pb_scenario, const Location& location) {
	for (auto& pb_e: pb_scenario.workers()) {
		Employee* e = new Employee(pb_e);
		string start = to_string(pb_e.startlocationid());
		string end = to_string(pb_e.endlocationid());
		e->AssignTracks(location.GetTrackByID(start), location.GetTrackByID(end));
		employees.push_back(e);
		debug_out("Imported Employee " << e->toString());
	}
	debug_out("finished loading employees from JSON");
}

template<class PBTrainGoal>
TrainGoal* ImportTrainGoal(const Location& location, const PBTrainGoal& m, bool in, bool standing) {
	TrainGoal* g = in ? static_cast<TrainGoal*>(new Incoming(m, standing)) : new Outgoing(m, standing);
	string park = to_string(m.parkingtrackpart());
	string side = to_string(m.sidetrackpart());
	g->assignTracks(location.GetTrackByID(park), location.GetTrackByID(side));
	return g;
}

void Scenario::ImportShuntingUnits(const PBScenario& pb_scenario, const Location& location) {
	for (auto& pb_train_type: pb_scenario.trainunittypes()) {
		TrainUnitType* tt = new TrainUnitType(pb_train_type);
		if (TrainUnitType::types.find(tt->displayName) != TrainUnitType::types.end())
			delete TrainUnitType::types.at(tt->displayName);
		TrainUnitType::types[tt->displayName] = tt;
	}
	for(auto& pb_in : pb_scenario.in())
		incomingTrains.push_back(dynamic_cast<Incoming*>(ImportTrainGoal(location, pb_in, true, false)));
	for(auto& pb_in : pb_scenario.instanding())
		incomingTrains.push_back(dynamic_cast<Incoming*>(ImportTrainGoal(location, pb_in, true, true)));
	for(auto& pb_out : pb_scenario.out())
		outgoingTrains.push_back(dynamic_cast<Outgoing*>(ImportTrainGoal(location, pb_out, false, false)));
	for(auto& pb_out : pb_scenario.outstanding())
		outgoingTrains.push_back(dynamic_cast<Outgoing*>(ImportTrainGoal(location, pb_out, false, true)));
}

