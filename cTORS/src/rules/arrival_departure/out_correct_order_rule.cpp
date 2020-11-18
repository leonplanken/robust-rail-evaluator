#include "BusinessRules.h"

/*

Rule that verifies that leaving shunting units have their train units in the 
correct order when they leave the shunting yard.

*/

pair<bool, string> out_correct_order_rule::IsValid(const State* state, const Action* action) const {
	if (auto ea = dynamic_cast<const ExitAction*>(action)) {
		auto& outTrains = ea->GetOutgoing()->GetShuntingUnit()->GetTrains();
		auto& avTrains = ea->GetShuntingUnit()->GetTrains();
		if(outTrains.size() != avTrains.size()) 
			return make_pair(false, "The candidate shunting unit " + ea->GetShuntingUnit()->toString() + " does not have the right number of trains (" + to_string(avTrains.size()) + " instead of the expted "+to_string(outTrains.size())+")");
		bool leftValid = true;
		bool rightValid = true;
		for (int i = 0; i < outTrains.size(); i++) {
			auto exp = outTrains.at(i);
			auto left = avTrains.at(i);
			auto right = avTrains.at(avTrains.size() - 1 - i);
			if ((exp->GetID() != -1 && exp->GetID() != left->GetID()) ||
				(exp->GetID() == -1 && exp->GetType()->displayName != left->GetType()->displayName))
				leftValid = false;
			if ((exp->GetID() != -1 && exp->GetID() != right->GetID()) ||
				(exp->GetID() == -1 && exp->GetType()->displayName != right->GetType()->displayName))
				rightValid = false;	
			if(!leftValid && !rightValid)
				return make_pair(false, "Leaving shunting unit " + ea->GetShuntingUnit()->toString() + "  did not match the train unit order of outgoing train " + ea->GetOutgoing()->GetShuntingUnit()->toString());
		}
	}
	return make_pair(true, "");
}

