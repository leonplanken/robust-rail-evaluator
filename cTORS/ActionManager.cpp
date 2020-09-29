#include "Action.h"

#ifndef ADD_GENERATOR
#define ADD_GENERATOR(generator)\
if (config->IsGeneratorActive(#generator)) { \
generators.push_back(new generator(config->GetActionParameters(#generator))); \
}
#endif

ActionManager::~ActionManager() {
	generators.clear();
}

void ActionManager::AddGenerators() {
	ADD_GENERATOR(ArriveActionGenerator);
	ADD_GENERATOR(ExitActionGenerator)
	ADD_GENERATOR(BeginMoveActionGenerator);
	ADD_GENERATOR(EndMoveActionGenerator);
	ADD_GENERATOR(MoveActionGenerator);
	ADD_GENERATOR(WaitActionGenerator);
	ADD_GENERATOR(ServiceActionGenerator);
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



