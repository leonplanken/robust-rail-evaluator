#include "Plan.h"

int POSAction::newUID{ 0 };

const ShuntingUnit GetShuntingUnitFromTrainIDs(const Scenario* scenario, PBList<string> pb_train_ids) {
    vector<string> trainIDs(pb_train_ids.begin(), pb_train_ids.end());
    vector<const Train*> trains (trainIDs.size());
    transform(trainIDs.begin(), trainIDs.end(), trains.begin(),
        [scenario](const string& s) -> const Train* {return scenario->GetTrainByID(stoi(s));});
    static int newSUID = 0;
    return ShuntingUnit(newSUID++, trains);
}

POSAction& POSAction::operator=(const POSAction& pa) {
    if(this != &pa) {
        id = pa.id;
        suggestedStart = pa.suggestedStart;
        suggestedEnd = pa.suggestedEnd;
        minDuration = pa.minDuration;
        delete action;
        action = pa.action->Clone();
    }
    return *this;
}

POSAction POSAction::CreatePOSAction(const Location* location, const Scenario* scenario, const PBPOSAction& pb_action) {
    int suggestedStartingTime = pb_action.suggestedstartingtime();
    int suggestedEndingTime = pb_action.suggestedfinishingtime();
    int minDuration = pb_action.minimumduration();
    auto su = GetShuntingUnitFromTrainIDs(scenario, pb_action.trainunitids());
    SimpleAction* action;
    if(pb_action.has_movement()) {
        auto track_id = *(pb_action.movement().path().end()-1);
        const Track* destination = location->GetTrackByID(to_string(track_id));
        action = new Move(su, destination); //TODO change to multi-move
    } else if(pb_action.has_task()) {
        auto& taskType = pb_action.task().type();
        if(taskType.has_predefined()) {
            auto task = taskType.predefined();
            switch(task) {
                case PBPredefinedTaskType::Arrive: 
                    action = new Arrive(scenario->GetIncomingBySU(&su)); break;
                case PBPredefinedTaskType::Exit: 
                    action = new Exit(scenario->GetOutgoingBySU(&su), su); break;
                case PBPredefinedTaskType::Split:
                {
                    // TODO how is the split action defined in protobuf? Current implementation: store the IDs of the first train.
                    vector<string> train_ids(pb_action.task().trainunitids().begin(), pb_action.task().trainunitids().end());
                    int splitIndex = 1;
                    for(auto& s: train_ids) {
                        int ix = su.GetTrainIndexByID(stoi(s));
                        if(ix == -1)
                            throw invalid_argument("Train with id " + s + " not found in shunting unit " + su.toString());
                        splitIndex = ix + 1 > splitIndex ? ix + 1 : splitIndex;
                    }
                    action = new Split(su, splitIndex);
                    break;
                }
                case PBPredefinedTaskType::Combine:
                {
                    // TODO how is the combine action defined in protobuf? Current implementation: store the IDs of the second train.
                    auto su2 = GetShuntingUnitFromTrainIDs(scenario, pb_action.task().trainunitids());
                    action = new Combine(su, su2); break;
                }
                case PBPredefinedTaskType::Walking:
                    action = new Setback(su); break;
                case PBPredefinedTaskType::Break:
                    action = new Wait(su); break;
                case PBPredefinedTaskType::NonService:
                    throw invalid_argument("Non-service tasks not implemented.");
                case PBPredefinedTaskType::Move:
                    throw invalid_argument("Move task type not used. Use POSMovement instead.");
                default:
                    throw invalid_argument("Invalid predefined task type " + to_string(task));
            }
        } else {
            const Train* train = su.GetTrainByID(stoi(pb_action.task().trainunitids().at(0)));
            auto tasks = scenario->GetTasksForTrain(train);
            const Task* task;
            for(auto& t: tasks) {
                if(t.taskType == taskType.other()) task = &t;
            }
            const Facility* facility = location->GetFacilityByID(pb_action.task().facilities().at(0).id());
            action = new Service(su, task, train, facility);
        }
    } else {
        action = new Wait(su);
    } 
    return POSAction(suggestedStartingTime, suggestedEndingTime, minDuration, action); 
}

void POSAction::Serialize(const Engine& engine, const State* state, PBPOSAction* pb_action) const {
    pb_action->set_suggestedstartingtime(suggestedStart);
    pb_action->set_suggestedfinishingtime(suggestedEnd);
    pb_action->set_minimumduration(minDuration);
    debug_out("Serialize train ids");
    for(auto t: action->GetShuntingUnit().GetTrains()) {
        *(pb_action->add_trainunitids()) = to_string(t->GetID());
    }
    if(instanceof<Move>(action)) {
        debug_out("Serialize move action");
        auto move = dynamic_cast<const Move*>(action);
        auto pb_move = pb_action->mutable_movement();
        auto path = engine.GetPath(state, *move);
        for(auto t: path.route) {
            pb_move->add_path(stoi(t->GetID()));
        }
        auto current = state->GetPosition(&move->GetShuntingUnit());
        auto previous = state->GetPrevious(&move->GetShuntingUnit());
        auto destination = move->GetDestination();
        auto prev_destination = prev(path.route.back());
        pb_move->set_fromside(current->IsASide(previous) ? PBSide::B : PBSide::A );
        pb_move->set_toside(destination->IsASide(prev_destination) ? PBSide::A : PBSide::B );
        pb_move->set_order(0);
    } else if(instanceof<Wait>(action)) {
        auto pb_wait = pb_action->mutable_break_();
    } else {
        auto pb_task = pb_action->mutable_task();
        auto pb_task_type = pb_task->mutable_type();
        if(instanceof<Service>(action)) {
            auto service = dynamic_cast<const Service*>(action);
            pb_task_type->set_other(service->GetTask()->taskType);
            pb_task->set_location(stoi(service->GetFacility()->GetTracks().at(0)->GetID()));
            auto pb_facility = pb_task->add_facilities();
            pb_facility->set_id(service->GetFacility()->GetID());
            pb_task->add_trainunitids(to_string(service->GetTrain()->GetID()));
        } else if(instanceof<Split>(action)) {
            pb_task_type->set_predefined(PBPredefinedTaskType::Split);
            auto split = dynamic_cast<const Split*>(action);
            debug_out("Serialize split action " + split->toString() + " #trains: " + to_string(split->GetShuntingUnit().GetTrains().size()));
            // TODO how is the split action defined in protobuf? Current implementation: store the IDs of the first train.
            for(int i=0; i<split->GetSplitIndex(); i++) {
                debug_out("Serialize split action: " + to_string(i));
                pb_task->add_trainunitids(to_string(split->GetShuntingUnit().GetTrains().at(i)->GetID()));
            } 
        } else if(instanceof<Combine>(action)) {
            pb_task_type->set_predefined(PBPredefinedTaskType::Combine);
            auto combine = dynamic_cast<const Combine*>(action);
            // TODO how is the combine action defined in protobuf? Current implementation: store the IDs of the second train.
            for(auto t: combine->GetSecondShuntingUnit().GetTrains())
                pb_task->add_trainunitids(to_string(t->GetID()));
        } else if(instanceof<Setback>(action)) {
            pb_task_type->set_predefined(PBPredefinedTaskType::Walking);
        } else if(instanceof<Arrive>(action)) {
            pb_task_type->set_predefined(PBPredefinedTaskType::Arrive);
        } else if(instanceof<Exit>(action)) {
            pb_task_type->set_predefined(PBPredefinedTaskType::Exit);
        } else {
            throw invalid_argument("Action serialization not implemented for action " + action->toString());
        }
    }
}

POSPlan POSPlan::CreatePOSPlan(const Location* location, const Scenario* scenario, const PBPOSPlan& pb_plan) {
    vector<PBPOSAction> pb_actions(pb_plan.actions().begin(), pb_plan.actions().end());
    vector<POSAction> actions;
    transform(pb_actions.begin(), pb_actions.end(), back_inserter(actions),
        [location, scenario](const PBPOSAction& pba) -> const POSAction {return POSAction::CreatePOSAction(location, scenario, pba);});
    return POSPlan(actions);
}

void POSPlan::Serialize(Engine& engine, const Scenario& scenario, PBPOSPlan* pb_plan) const {
    auto state = engine.StartSession(scenario);
    auto it = actions.begin();
    while(it != actions.end()) {
        try {
            debug_out("Serializing T=" + to_string(state->GetTime()) + ". A=" + it->GetAction()->toString() + ".");
            engine.Step(state);
            debug_out("Finished Step Update");
            if(state->GetTime() >= it->GetSuggestedStart()) {
                if(!instanceof<BeginMove>(it->GetAction()) && !instanceof<EndMove>(it->GetAction())) {
                    auto pb_action = pb_plan->add_actions();
                    debug_out("Serialize action");
                    it->Serialize(engine, state, pb_action);
                    debug_out("Finish Serialize action");
                }
                debug_out("Apply action");
                engine.ApplyAction(state, *(it->GetAction()));
                debug_out("End action");
                it++;
            }
        } catch (ScenarioFailedException e) {
			cout << "Scenario failed.\n";
            pb_plan->set_feasible(false);
			break;
		}
    }
    pb_plan->set_feasible(state->GetShuntingUnits().size() == 0);
    engine.EndSession(state);
}

void POSPlan::SerializeToFile(Engine& engine, const Scenario& scenario, const string& outfile) const {
    PBPOSPlan pb_plan;
    debug_out("Start Serializing plan.")
    Serialize(engine, scenario, &pb_plan);
    debug_out("End Serializing plan.")
    parse_pb_to_json(outfile, pb_plan);
}