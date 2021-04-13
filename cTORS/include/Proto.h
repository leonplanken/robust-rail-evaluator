#ifndef PROTO_H
#define PROTO_H
#include <string>
#include <vector>
#include <list>
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include "Location.pb.h"
#include "Scenario.pb.h"
#include "Vehicles.pb.h"
#include "Employee.pb.h"
#include "TrainUnitTypes.pb.h"
#include "PartialOrderSchedule.pb.h"
using namespace std;

typedef algo_iface::Scenario PBScenario;
typedef algo_iface::Train PBTrainGoal;

typedef tors_proto::IncomingTrain PBIncoming;
typedef tors_proto::OutgoingTrain PBOutgoing;
typedef tors_proto::ShuntingUnit PBShuntingUnit;

typedef tors_proto::TrainUnit PBTrainUnit;
typedef algo_iface::TrainUnit PBSTrainUnit;
typedef algo_iface::TrainUnitType PBTrainUnitType;
typedef algo_iface::TaskType PBTaskType;
typedef algo_iface::PredefinedTaskType PBPredefinedTaskType;
typedef algo_iface::TaskSpec PBSTask;
typedef tors_proto::Task PBTask;

typedef tors_proto::Employee PBEmployee;
typedef algo_iface::MemberOfStaff PBMemberOfStaff;
typedef algo_iface::TimeInterval PBTimeInterval;
typedef tors_proto::Interval PBInterval;

typedef algo_iface::Location PBLocation;
typedef algo_iface::Facility PBFacility;
typedef algo_iface::TrackPart PBTrack;
typedef algo_iface::TrackPartType PBTrackPartType;
typedef algo_iface::Side PBSide;

typedef algo_iface::POSPlan PBPOSPlan;
typedef algo_iface::POSMatch PBPOSMatch;
typedef algo_iface::POSAction PBPOSAction;
typedef algo_iface::POSMovement PBPOSMovement;
typedef algo_iface::POSTask PBPOSTask;
typedef algo_iface::POSBreak PBPOSBreak;
typedef algo_iface::POSPrecedenceConstraint PBPOSPrecedenceConstraint;

typedef google::protobuf::uint64 UInt;
template<class Item> using PBList =  google::protobuf::RepeatedPtrField<Item>;

inline vector<string> PBToStringVector(const PBList<string>& items) {
    return vector<string>(items.begin(), items.end());
}

inline list<string> PBToStringList(const PBList<string>& items) {
    return list<string>(items.begin(), items.end());
}

#endif