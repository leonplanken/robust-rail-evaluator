#include "doctest/doctest.h"
#include "Engine.h"

namespace cTORSTest
{
	TEST_CASE("Engine Test") {
		Engine engine("data/Demo");

		SUBCASE("Test create state") {
			auto state = engine.StartSession();
			SUBCASE("Test active actions") {
				ShuntingUnit su;
				state->AddShuntingUnit(&su);
				BeginMoveAction beginMoveAction(&su, 25);
				state->AddActiveAction(&su, &beginMoveAction);
				CHECK(state->HasActiveAction(&su));
				state->RemoveActiveAction(&su, &beginMoveAction);
				CHECK(!state->HasActiveAction(&su));
			}
			SUBCASE("Test get actions") {
				list<Action*> &actions = engine.GetActions(state);
				CHECK(actions.size() > 0);
				Action* a = actions.front();
				engine.ApplyAction(state, a);
			}
			engine.EndSession(state);
		}

		SUBCASE("Test get scenario") {
			auto scenario = engine.GetScenario();
		}
		
	}
}
