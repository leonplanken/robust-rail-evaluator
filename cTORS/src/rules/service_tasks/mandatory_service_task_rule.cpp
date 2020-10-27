#include "BusinessRules.h"

/*

Rule that verifies that all required service tasks are performed before a shunting unit leaves the shunting yard.

*/

pair<bool, string> mandatory_service_task_rule::IsValid(State* state, Action* action) const {
	if (ExitAction* ea = dynamic_cast<ExitAction*>(action)) {
		auto su = ea->GetShuntingUnit();
		for (auto tu : su->GetTrains()) {
			auto& tasks = state->GetTasksForTrain(tu);
			for (auto& task : tasks) {
				if (task.priority == 0)
					return make_pair(false, "Shunting unit " + su->toString() + " cannot leave, because train " +tu->toString() + "  still has the following unfinished task: " + task.toString() + ".");
			}
		}
	}
	return make_pair(true, "");
}

