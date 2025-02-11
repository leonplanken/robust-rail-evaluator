#include "doctest/doctest.h"
#include "Engine.h"

#include <google/protobuf/util/json_util.h>

namespace cTORSTest
{

	TEST_CASE("Actions test")
	{
		cout << "-------------------------------------------------------------------------------------------------" << endl;
		cout << "											ACTION TEST 										 ";
		cout << "-------------------------------------------------------------------------------------------------" << endl;

		LocationEngine engine("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test");
		auto &scenario = engine.GetScenario("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test/scenario.json");

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
		GetRunResultProto("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test/plan_bis.json", pb_run_external);

		auto runResult_external = RunResult::CreateRunResult(&location, pb_run_external);
		CHECK(engine.EvaluatePlan(runResult_external->GetScenario(), runResult_external->GetPlan()));

		cout << "EvaluatePlan passed" << endl;

		// Create jsomn foramt plan

		// string jsonResult;
		// google::protobuf::util::Status status = google::protobuf::util::MessageToJsonString(pb_run, &jsonResult);

		// if (status.ok())
		// {
		// 	std::cout << "JSON string: " << jsonResult << std::endl;
		// }
		// else
		// {
		// 	std::cerr << "Failed to convert protobuf to JSON: " << status.ToString() << std::endl;
		// }

		// delete runResult;
		delete runResult_external;
	}
}
