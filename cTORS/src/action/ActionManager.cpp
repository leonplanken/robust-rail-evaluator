#include "Action.h"

#ifndef ADD_GENERATOR
#define ADD_GENERATOR(name, generator)\
if (config->IsGeneratorActive(name)) { \
generators.push_back(new generator(config->GetActionParameters(name))); \
}
#endif

ActionManager::~ActionManager() {
	generators.clear();
}

void ActionManager::AddGenerators() {
	ADD_GENERATOR("arrive", ArriveActionGenerator);
	ADD_GENERATOR("exit", ExitActionGenerator)
	ADD_GENERATOR("begin_move", BeginMoveActionGenerator);
	ADD_GENERATOR("end_move", EndMoveActionGenerator);
	ADD_GENERATOR("move", MoveActionGenerator);
	ADD_GENERATOR("wait", WaitActionGenerator);
	ADD_GENERATOR("service", ServiceActionGenerator);
}

void ActionManager::AddGenerator(string name, ActionGenerator* generator) {
	if (config->IsGeneratorActive(name)) {
		generators.push_back(generator);
	}
}

void ActionManager::Generate(State* state, list<Action*>& out) const
{
	for (auto generator : generators) {
		generator->Generate(state, out);
	}
}



