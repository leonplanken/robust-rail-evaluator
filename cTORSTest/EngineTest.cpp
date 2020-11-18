#include "doctest/doctest.h"
#include "Engine.h"

namespace cTORSTest
{
	TEST_CASE("Engine Test") {
		Engine engine("data/Demo");
		auto& tracks = engine.GetLocation().GetTracks();
		auto& scenario = engine.GetScenario();
		Track* railTrack;
		auto it  = find_if(tracks.begin(), tracks.end(),
				[](auto t) -> bool { return t->GetType()==TrackPartType::Railroad; });
		{
			REQUIRE(it!=tracks.end());
			railTrack = *it;
		}
		auto previous = railTrack->GetNeighbors().front();
		SUBCASE("Test create state") {
			auto state = engine.StartSession();
			SUBCASE("Test active actions") {
				ShuntingUnit* su = new ShuntingUnit(0, {new Train(0, TrainUnitType::types.begin()->second)});
				state->AddShuntingUnit(su, railTrack, previous);
				BeginMoveAction* beginMoveAction = new BeginMoveAction(su, 25);
				state->AddActiveAction(su, beginMoveAction);
				CHECK(state->HasActiveAction(su));
				state->RemoveActiveAction(su, beginMoveAction);
				CHECK(!state->HasActiveAction(su));
			}
			SUBCASE("Test get actions") {
				list<const Action*> &actions = engine.GetActions(state);
				CHECK(actions.size() > 0);
				auto a = actions.front();
				engine.ApplyAction(state, a);
			}
			engine.EndSession(state);
		}		
	}
}
