#include "doctest/doctest.h"
#include "Engine.h"

#include <google/protobuf/util/json_util.h>

namespace cTORSTest
{

	TEST_CASE("Scenario and Location Compatibility test")
	{

		cout << "-------------------------------------------------------------------------------------------------" << endl;
		cout << "											SCENARIO AND LOCATION TEST 										 ";
		cout << "-------------------------------------------------------------------------------------------------" << endl;
		// LocationEngine engine("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test");

		// LocationEngine engine("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator");
		LocationEngine engine("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2");


		cout << "------------------------------------------------------" << endl;
		cout << "               Location file loading is done" << endl;
		cout << "------------------------------------------------------" << endl;

		// auto &sc1 = engine.GetScenario("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test/scenario.json");
		// auto &sc1 = engine.GetScenario("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/scenario.json");
		auto &sc1 = engine.GetScenario("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/scenario.json");
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

	TEST_CASE("Plan Compatibility test")
	{
		/* HIP plan protobuf is different thatn the one used by cTORS. HIP plans converted into json format that has to be somehow parsed to cTORS protobuf
		this code is intended to do this conversion*/

		// LocationEngine engine("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test");
		// auto &scenario = engine.GetScenario("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test/scenario.json");
		// const Location &location = engine.GetLocation();

		LocationEngine engine("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/");
		auto &scenario = engine.GetScenario("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/scenario.json");
		const Location &location = engine.GetLocation();

		PB_HIP_Plan pb_hip_plan;

		// ParseHIP_PlanFromJson("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/hip_test/hip_based_plan.json", pb_hip_plan);

		ParseHIP_PlanFromJson("/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/plan.json", pb_hip_plan);

	

		auto runResult_external = RunResult::CreateRunResult(pb_hip_plan, "/home/roland/Documents/REIT/LPT_Robust_Rail_project/cTORS_new/ctors/data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/scenario.json", &location);
		CHECK(engine.EvaluatePlan(runResult_external->GetScenario(), runResult_external->GetPlan()));

	}
}