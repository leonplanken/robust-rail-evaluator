#pragma once
#include <list>
#include <string>
#include <filesystem>
#include <fstream>
#include "EventQueue.h"
#include "Location.h"
#include "Exceptions.h"
#include "Employee.h"
#include "TrainGoals.h"
#include "Utils.h"
namespace fs = std::filesystem;
using namespace std;
using json = nlohmann::json;

class Scenario
{
private:
	static const string scenarioFileString;

	int startTime, endTime;
	EventQueue disturbances;
	vector<Employee*> employees;
	vector<Incoming*> incomingTrains;
	vector<Outgoing*> outgoingTrains;
	vector<Event*> disturbanceList;

	void importEmployeesFromJSON(const json& j, const Location& location);
	void importShuntingUnitsFromJSON(const json& j, const Location& location);

public:
	Scenario();
	Scenario(string path, const Location& location);
	Scenario(const Scenario& scenario);
	~Scenario();
	
	inline int GetStartTime() const { return startTime; }
	inline int GetEndTime() const { return endTime; }
	inline const EventQueue& GetDisturbances() const { return disturbances; }
	inline const vector<Event*>& GetDisturbanceVector() const { return disturbanceList; }
	inline const vector<Outgoing*>& GetOutgoingTrains() const { return outgoingTrains; }
	inline const vector<Incoming*>& GetIncomingTrains() const {	return incomingTrains; }
	inline const vector<Employee*>& GetEmployees() const { return employees; }
	const size_t GetNumberOfTrains() const;

	inline void SetStartTime(int startTime) { this->startTime = startTime; }
	inline void SetEndTime(int endTime) { this->endTime = endTime; }
	inline void SetOutgoingTrains(vector<Outgoing*> outgoingTrains) { this->outgoingTrains = outgoingTrains; }
	inline void SetIncomingTrains(vector<Incoming*> incomingTrains) { this->incomingTrains = incomingTrains; }
	inline void SetEmployees(vector<Employee*> employees) { this->employees = employees; }
	inline void SetDisturbances(EventQueue disturbances) { this->disturbances = disturbances; }

	inline void AddOutgoingTrain(Outgoing* outgoingTrain) { outgoingTrains.push_back(outgoingTrain); }
	inline void AddIncomingTrain(Incoming* incomingTrain) { incomingTrains.push_back(incomingTrain); }
	inline void AddDisturbance(Event* disturbance) { disturbances.push(disturbance); }
	inline void AddEmployee(Employee* employee) { employees.push_back(employee); }
};

