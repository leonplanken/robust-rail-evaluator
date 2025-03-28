#include "doctest/doctest.h"
#include "Engine.h"

#include <google/protobuf/util/json_util.h>

namespace cTORSTest
{

	// To specify the scenario / location / plan - PATH
	// In the terminal the followinf should be exported by respecting the argument names
	// 
	// export LOCATION_PATH="/path/to/location_folder" - where the location.json file can be found e.g., "~/data/Demo/hip_test"
	// export SCENARIO_PATH="/path/to/scenario_folder/scenario.json"
	// export PLAN_PATH="/path/to/plan_folder/plan.json"

	string location_path;
	string scenario_path;
	string plan_path;

	TEST_CASE("Scenario and Location test")
	{


		
		cout << "-------------------------------------------------------------------------------------------------" << endl;
		cout << "											SCENARIO AND LOCATION TEST 										 ";
		cout << "-------------------------------------------------------------------------------------------------" << endl;
				

		// Get location path and ensure it is specified 
		const char* LOCATION_PATH = getenv("LOCATION_PATH");
		REQUIRE(LOCATION_PATH != nullptr);

		location_path = string(LOCATION_PATH);
		cout << "Location path: " << location_path << endl;

		// Get scenario path and ensure it is specified 
		const char* SCENARIO_PATH = getenv("SCENARIO_PATH");
		REQUIRE(SCENARIO_PATH != nullptr);

		scenario_path = string(SCENARIO_PATH);
		cout << "Scenario path: " << scenario_path << endl;


		// Get scenario path and ensure it is specified 
		const char* PLAN_PATH = getenv("PLAN_PATH");
		REQUIRE(PLAN_PATH != nullptr);

		plan_path = string(PLAN_PATH);
		cout << "Plan path: " << plan_path << endl;
		


		LocationEngine engine(location_path);


		cout << "------------------------------------------------------" << endl;
		cout << "               Location file loading is done" << endl;
		cout << "------------------------------------------------------" << endl;

		auto &sc1 = engine.GetScenario(scenario_path);
		{
			Scenario sc2(sc1);
		}
		CHECK(sc1.GetIncomingTrains().front()->GetShuntingUnit()->GetTrains().front().GetType() != nullptr);
		auto st1 = engine.StartSession(sc1);
		engine.EndSession(st1);
		CHECK(sc1.GetIncomingTrains().front()->GetShuntingUnit()->GetTrains().front().GetType() != nullptr);
		// for (int i = 0; i < 1; i++)
		// {
		// 	CAPTURE("Test " + to_string(i));
		// 	Scenario sc3(sc1);
		// 	auto st2 = engine.StartSession(sc3);
		// 	engine.Step(st2);
		// 	int counter = 0;
		// 	while (true)
		// 	{
		// 		try
		// 		{
		// 			list<const Action *> &actions = engine.GetValidActions(st2);
		// 			if (actions.size() == 0)
		// 				break;
		// 			auto it = actions.begin();
		// 			advance(it, counter++ % actions.size());
		// 			auto a = *it;
		// 			engine.ApplyActionAndStep(st2, a);
		// 		}
		// 		catch (ScenarioFailedException &e)
		// 		{
		// 			break;
		// 		}
		// 	}
		// 	engine.EndSession(st2);
		// 	CHECK(sc1.GetIncomingTrains().front()->GetShuntingUnit()->GetTrains().front().GetType() != nullptr);
		// }
	}




	TEST_CASE("Plan test")
	{
		cout << "-------------------------------------------------------------------------------------------------" << endl;
		cout << "											PLAN TEST 										 ";
		cout << "-------------------------------------------------------------------------------------------------" << endl;

		LocationEngine engine(location_path);
		auto &scenario = engine.GetScenario(scenario_path);

		const Location &location = engine.GetLocation();
		const vector<Track *> &tracks = location.GetTracks();

		cout << "\n Location \n";

		for (int i = 0; i < tracks.size(); i++)
		{
			cout << "id : " << tracks[i]->id << "\n";
			if (tracks[i]->type == TrackPartType::Railroad)
			{
				cout << "Type : RailRoad" << "\n";
			}
			cout << "name : " << tracks[i]->name << "\n";
		}

		PBRun pb_run_external;

		// Test embedded hip plan conversion
		GetRunResultProto(plan_path, pb_run_external);

		auto runResult_external = RunResult::CreateRunResult(&location, pb_run_external);
		CHECK(engine.EvaluatePlan(runResult_external->GetScenario(), runResult_external->GetPlan()));

		cout << "EvaluatePlan passed" << endl;

		// delete runResult;
		delete runResult_external;
	}
}
