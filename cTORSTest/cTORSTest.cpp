#include "pch.h"
#include "CppUnitTest.h"
#include "Engine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cTORSTest
{
	TEST_CLASS(cTORSTest)
	{
	public:
		
		TEST_METHOD(TestLoadEngine)
		{
			Engine engine("../data/Demo");
		}

		TEST_METHOD(TestCreateState)
		{
			Engine engine("../data/Demo");
			engine.StartSession();
		}

		TEST_METHOD(TestActiveActions)
		{
			Engine engine("../data/Demo");
			State* state = engine.StartSession();
			ShuntingUnit su;
			state->AddShuntingUnit(&su);
			BeginMoveAction beginMoveAction(&su, 25);
			state->AddActiveAction(&su, &beginMoveAction);
			Assert::IsTrue(state->HasActiveAction(&su));
			state->RemoveActiveAction(&su, &beginMoveAction);
			Assert::IsTrue(!state->HasActiveAction(&su));
		}
	};
}
