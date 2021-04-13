#ifndef PLAN_H
#define PLAN_H
#include <list>
#include <vector>
#include "Action.h"
#include "Scenario.h"
#include "Utils.h"
#include "Engine.h"

class Engine;

class POSAction {
private:
    static int newUID;
    int id;
    int suggestedStart, suggestedEnd, minDuration;
    const SimpleAction* action;
public:
    POSAction() = delete;
    POSAction(int suggestedStart, int suggestedEnd, int minDuration, const SimpleAction* action) :
        id(newUID++), suggestedStart(suggestedStart), suggestedEnd(suggestedEnd), minDuration(minDuration), action(action) {}
    POSAction(const POSAction& pa) : id(pa.id), suggestedStart(pa.suggestedStart), suggestedEnd(pa.suggestedEnd),
        minDuration(pa.minDuration), action(pa.action->Clone()) {}
    ~POSAction() { delete action; }
    POSAction& operator=(const POSAction& pa);
    inline int GetID() const { return id; }
    inline int GetSuggestedStart() const { return suggestedStart; }
    inline int GetSuggestedEnd() const { return suggestedEnd; }
    inline int GetMinimumDuration() const { return minDuration; }
    inline const SimpleAction* GetAction() const { return action; }
    void Serialize(const Engine& engine, const State* state, PBPOSAction* pb_action) const;
    static POSAction CreatePOSAction(const Location* location, const Scenario* scenario, const PBPOSAction& pb_action);
};

class POSMatch {
private:
    const Train* train;
    const Outgoing* out;
    int position;
};

class POSPrecedenceConstraint {
    const POSAction *firstAction, *secondAction;
    int minimumTimeLag;
};

class POSPlan {
private:
    vector<POSAction> actions;
    vector<POSMatch> matching;
    vector<POSPrecedenceConstraint> graph;
    bool feasible;
public:
    POSPlan() = default;
    POSPlan(vector<POSAction> actions) : actions(actions) {}
    inline const vector<POSAction>& GetActions() const { return actions; }
    inline void AddAction(const POSAction& action) { actions.push_back(action); }
    void Serialize(Engine& engine, const Scenario& scenario, PBPOSPlan* pb_plan) const;
    void SerializeToFile(Engine& engine, const Scenario& scenario, const string& outfile) const;
    static POSPlan CreatePOSPlan(const Location* location, const Scenario* scenario, const PBPOSPlan& pb_plan);
};



#endif