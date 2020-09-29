#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Engine.h"

#ifndef BIND_ACTION
#define BIND_ACTION(name) \
	py::class_<name, Action>(m, #name) \
	.def("__str__", &name::toString);
#endif


namespace py = pybind11;
using namespace std;



PYBIND11_MODULE(pyTORS, m) {
    m.doc() = "TORS implemented in C++"; 

	////////////////////////////////////
	//// ShuntingUnit               ////
	////////////////////////////////////
	py::class_<ShuntingUnit>(m, "ShuntingUnit")
		.def(py::init<int, vector<Train*>>())
		.def_property("train_units", &ShuntingUnit::GetTrains, &ShuntingUnit::SetTrains, py::return_value_policy::reference, py::keep_alive<1, 2>())
		.def("get_length", &ShuntingUnit::GetLength)
		.def("needs_electricity", &ShuntingUnit::NeedsElectricity)
		.def("__str__", &ShuntingUnit::toString)
		.def("get_number_of_trains", &ShuntingUnit::GetNumberOfTrains)
		.def("__equals__", &ShuntingUnit::operator==, py::arg("other"))
		.def("get_trains", &ShuntingUnit::GetTrains, py::return_value_policy::reference)
		.def("get_copy", [](const ShuntingUnit& su) { return new ShuntingUnit(su); }, py::return_value_policy::take_ownership);

	////////////////////////////////////
	//// Train                      ////
	////////////////////////////////////
	py::class_<Train>(m, "Train")
		.def(py::init<int, TrainUnitType*>())
		.def_property("id", &Train::GetID, &Train::SetID)
		.def_property_readonly("type", &Train::GetType, py::return_value_policy::reference)
		.def("get_type", &Train::GetType, py::return_value_policy::reference)
		.def("get_id", &Train::GetID)
		.def("__str__", &Train::toString)
		.def("__equals__", &Train::operator==, py::arg("other"))
		.def("get_copy", [](const Train& tu) { return new Train(tu); }, py::return_value_policy::take_ownership);

	////////////////////////////////////
	//// TrainUnitType              ////
	////////////////////////////////////
	py::class_<TrainUnitType>(m, "TrainUnitType")
		.def(py::init<const string&, int, double, int, int, int,
			int, int, int, const string&, bool, bool, bool>())
		.def_readonly("display_name", &TrainUnitType::displayName)
		.def_readonly("carriages", &TrainUnitType::carriages)
		.def_readonly("length", &TrainUnitType::length)
		.def_readonly("combine_duration", &TrainUnitType::combineDuration)
		.def_readonly("split_duration", &TrainUnitType::splitDuration)
		.def_readonly("back_norm_time", &TrainUnitType::backNormTime)
		.def_readonly("back_addition_time", &TrainUnitType::backAdditionTime)
		.def_readonly("travel_speed", &TrainUnitType::travelSpeed)
		.def_readonly("startup_time", &TrainUnitType::startUpTime)
		.def_readonly("type_prefix", &TrainUnitType::typePrefix)
		.def_readonly("needs_loco", &TrainUnitType::needsLoco)
		.def_readonly("is_loco", &TrainUnitType::isLoco)
		.def_readonly("needs_electricity", &TrainUnitType::needsElectricity)
		.def("__str__", &TrainUnitType::toString)
		.def("__equals__", &TrainUnitType::operator==);

	////////////////////////////////////
	//// Task                       ////
	////////////////////////////////////
	py::class_<Task>(m, "Task")
		.def(py::init<const string&, int, int, list<string>>())
		.def_readonly("task_type", &Task::taskType)
		.def_readonly("priority", &Task::priority)
		.def_readonly("duration", &Task::duration)
		.def_readonly("skills", &Task::skills, py::return_value_policy::copy)
		.def("__str__", &Task::toString)
		.def("__equals__", &Task::operator==);

	////////////////////////////////////
	//// Incoming, Outgoing         ////
	////////////////////////////////////
	py::class_<TrainGoal>(m, "TrainGoal")
		.def_property_readonly("shunting_unit", &TrainGoal::GetShuntingUnit)
		.def_property_readonly("time", &TrainGoal::GetTime)
		.def("is_instanding", &TrainGoal::IsInstanding)
		.def("get_time", &TrainGoal::GetTime)
		.def("get_standing_index", &TrainGoal::GetStandingIndex)
		.def("get_shunting_unit", &TrainGoal::GetShuntingUnit, py::return_value_policy::reference)
		.def("get_side_track", &TrainGoal::GetSideTrack, py::return_value_policy::reference)
		.def("get_parking_track", &TrainGoal::GetParkingTrack, py::return_value_policy::reference);
	py::class_<Incoming, TrainGoal>(m, "Incoming")
		.def(py::init<int, ShuntingUnit*, Track*, Track*, int, bool, int, map<Train*, vector<Task>>>())
		.def("__str__", &Incoming::toString);
	py::class_<Outgoing, TrainGoal>(m, "Outgoing")
		.def(py::init<int, ShuntingUnit*, Track*, Track*, int, bool, int>())
		.def("__str__", &Outgoing::toString);

	////////////////////////////////////
	//// State                      ////
	////////////////////////////////////
	py::class_<State>(m, "State")
		.def_property_readonly("time", &State::GetTime)
		.def_property_readonly("start_time", &State::GetStartTime)
		.def_property_readonly("end_time", &State::GetEndTime)
		.def("peek_event", &State::PeekEvent, py::return_value_policy::reference)
		.def("get_incoming_trains", &State::GetIncomingTrains, py::return_value_policy::reference)
		.def("get_outgoing_trains", &State::GetOutgoingTrains, py::return_value_policy::reference)
		.def("get_position", &State::GetPosition, py::arg("shunting_unit"), py::return_value_policy::reference)
		.def("get_previous", &State::GetPrevious, py::arg("shunting_unit"), py::return_value_policy::reference)
		.def("get_occupations", &State::GetOccupations, py::arg("track"), py::return_value_policy::reference)
		.def("get_position_on_track", &State::GetPositionOnTrack, py::arg("shunting_unit"))
		.def("get_amount_on_track", &State::GetAmountOnTrack, py::arg("track"))
		.def("can_move_to_side", &State::CanMoveToSide, py::arg("shunting_unit"), py::arg("track"))
		.def("is_moving", &State::IsMoving, py::arg("shunting_unit"))
		.def("is_waiting", &State::IsWaiting, py::arg("shunting_unit"))
		.def("is_reserved", &State::IsReserved, py::arg("track"))
		.def("get_shunting_units", &State::GetShuntingUnits, py::return_value_policy::reference)
		.def("has_active_action", &State::HasActiveAction, py::arg("shunting_unit"))
		.def("get_train_units_in_order", &State::GetTrainUnitsInOrder, py::arg("shunting_unit"), py::return_value_policy::move)
		.def("get_direction", &State::GetDirection, py::arg("shunting_unit"))
		.def("get_active_actions", &State::GetActiveActions, py::return_value_policy::reference)
		.def("get_tasks_for_train", &State::GetTasksForTrain, py::arg("train"), py::return_value_policy::reference)
		.def("get_active_tasks", &State::GetActiveTasks, py::return_value_policy::reference);

	////////////////////////////////////
	//// Action                     ////
	////////////////////////////////////
	py::class_<Action>(m, "Action")
		.def("__str__", &Action::toString)
		.def("get_shunting_unit", &Action::GetShuntingUnit, py::return_value_policy::reference)
		.def("get_reserved_tracks", &Action::GetReservedTracks, py::return_value_policy::reference)
		.def("get_duration", &Action::GetDuration)
		.def("__equals__ ", &Action::operator==);
	auto arriveAction = BIND_ACTION(ArriveAction);
	BIND_ACTION(BeginMoveAction);
	BIND_ACTION(EndMoveAction);
	auto moveAction = BIND_ACTION(MoveAction);
	auto exitAction = BIND_ACTION(ExitAction);
	BIND_ACTION(CombineAction);
	BIND_ACTION(SplitAction);
	auto serviceAction = BIND_ACTION(ServiceAction);
	auto setbackAction = BIND_ACTION(SetbackAction);
	BIND_ACTION(WaitAction);
	arriveAction.def("get_destination_track", &ArriveAction::GetDestinationTrack, py::return_value_policy::reference)
		.def("get_incoming", &ArriveAction::GetIncoming, py::return_value_policy::reference);
	moveAction.def("get_destination_track", &MoveAction::GetDestinationTrack, py::return_value_policy::reference)
		.def("get_previous_track", &MoveAction::GetPreviousTrack, py::return_value_policy::reference)
		.def("get_tracks", &MoveAction::GetTracks, py::return_value_policy::reference);
	exitAction.def("get_destination_track", &ExitAction::GetDestinationTrack, py::return_value_policy::reference)
		.def("get_outgoing", &ExitAction::GetOutgoing, py::return_value_policy::reference);
	serviceAction.def("get_train", &ServiceAction::GetTrain, py::return_value_policy::reference)
		.def("get_task", &ServiceAction::GetTask, py::return_value_policy::reference)
		.def("get_facility", &ServiceAction::GetFacility, py::return_value_policy::reference)
		.def("get_employees", &ServiceAction::GetEmployees, py::return_value_policy::reference);
	setbackAction.def("get_drivers", &SetbackAction::GetDrivers, py::return_value_policy::reference);
	
	////////////////////////////////////
	//// Employee                   ////
	////////////////////////////////////
	py::class_<Employee>(m, "Employee")
		.def("__str__", &Employee::toString);

	////////////////////////////////////
	//// Location                   ////
	////////////////////////////////////
	py::class_<Location>(m, "Location")
		.def("get_track_parts", &Location::GetTracks, py::return_value_policy::reference)
		.def("get_track_by_id", &Location::getTrackByID, py::arg("id"), py::return_value_policy::reference);

	py::enum_<TrackPartType>(m, "TrackPartType")
		.value("RAILROAD", TrackPartType::Railroad)
		.value("SWITCH", TrackPartType::Switch)
		.value("ENGLISH_SWITCH", TrackPartType::EnglishSwitch)
		.value("HALF_ENGLISH_SWITCH", TrackPartType::HalfEnglishSwitch)
		.value("INTERSECTION", TrackPartType::InterSection)
		.value("BUMPER", TrackPartType::Bumper)
		.value("BUILDING", TrackPartType::Building)
		.export_values();

	py::class_<Facility>(m, "Facility")
		.def("get_capacity", &Facility::GetCapacity)
		.def("get_tasks", &Facility::GetTasks, py::return_value_policy::reference)
		.def("is_available", &Facility::IsAvailable, py::arg("start_time"), py::arg("duration"))
		.def("executes_task", &Facility::ExecutesTask, py::arg("task"))
		.def("__str__", &Facility::toString);

	////////////////////////////////////
	//// Track                      ////
	////////////////////////////////////
	py::class_<Track>(m, "Track")
		.def_readonly("id", &Track::id)
		.def_readonly("type", &Track::type)
		.def_readonly("length", &Track::length)
		.def_readonly("name", &Track::name)
		.def_readonly("sawMovement_allowed", &Track::sawMovementAllowed)
		.def_readonly("parking_allowed", &Track::parkingAllowed)
		.def_readonly("is_electrified", &Track::isElectrified)
		.def_readonly("standing_allowed", &Track::standingAllowed)
		.def("__str__", &Track::toString)
		.def("is_a_side", &Track::IsASide, py::arg("track"))
		.def("is_b_side", &Track::IsBSide, py::arg("track"))
		.def("get_facilities", &Track::GetFacilities, py::return_value_policy::reference)
		.def("get_next_track_parts", &Track::GetNextTrackParts, py::arg("previous_track"), py::return_value_policy::reference)
		.def("get_neighbors", &Track::GetNeighbors, py::return_value_policy::reference)
		.def("__equals__", &Track::operator==, py::arg("track"));

	////////////////////////////////////
	//// Scenario                   ////
	////////////////////////////////////
	py::class_<Scenario>(m, "Scenario")
		.def(py::init<>())
		.def("get_start_time", &Scenario::GetStartTime)
		.def("get_end_time", &Scenario::GetEndTime)
		.def("set_start_time", &Scenario::SetStartTime, py::arg("start_time"))
		.def("set_end_time", &Scenario::SetEndTime, py::arg("end_time"))
		.def("get_number_of_trains", &Scenario::GetNumberOfTrains)
		.def("get_outgoing_trains", &Scenario::GetOutgoingTrains, py::return_value_policy::reference)
		.def("get_incoming_trains", &Scenario::GetIncomingTrains, py::return_value_policy::reference)
		.def("set_outgoing_trains", &Scenario::SetOutgoingTrains, py::arg("incoming_trains"), py::keep_alive<1, 2>())
		.def("set_incoming_trains", &Scenario::SetIncomingTrains, py::arg("outgoing_trains"), py::keep_alive<1, 2>())
		.def("add_outgoing_train", &Scenario::AddOutgoingTrain, py::arg("outgoing_train"), py::keep_alive<1, 2>())
		.def("add_incoming_train", &Scenario::AddIncomingTrain, py::arg("incoming_train"), py::keep_alive<1, 2>())
		.def("get_employees", &Scenario::GetEmployees, py::return_value_policy::reference)
		.def("set_employees", &Scenario::SetEmployees, py::arg("employees"), py::keep_alive<1, 2>())
		.def("add_employee", &Scenario::AddEmployee, py::arg("employee"), py::keep_alive<1, 2>())
		.def("get_disturbances", &Scenario::GetDisturbances, py::return_value_policy::reference)
		.def("get_disturbance_list", &Scenario::GetDisturbanceVector, py::return_value_policy::reference)
		.def("set_disturbances", &Scenario::SetDisturbances, py::arg("disturbances"), py::keep_alive<1, 2>())
		.def("add_disturbance", &Scenario::AddDisturbance, py::arg("disturbance"), py::keep_alive<1, 2>())
		.def("get_copy", [](const Scenario& scenario) { return new Scenario(scenario); }, py::return_value_policy::take_ownership);
	

	////////////////////////////////////
	//// Engine                     ////
	////////////////////////////////////
	py::class_<Engine>(m, "Engine")
		.def(py::init<const std::string&>())
		.def("get_actions", &Engine::GetActions, py::arg("state"), py::arg("scenario") = nullptr, py::return_value_policy::reference)
		.def("apply_action", &Engine::ApplyAction, py::arg("state"), py::arg("action"))
		.def("start_session", 
			[](Engine& e, Scenario *scenario) -> State* { 
				if (scenario == nullptr) return e.StartSession();
				else return e.StartSession(*scenario); 
		}, py::arg("scenario") = nullptr, py::return_value_policy::reference)
		.def("end_session", &Engine::EndSession, py::arg("state"))
		.def("get_location", &Engine::GetLocation, py::return_value_policy::reference)
		.def("get_scenario", &Engine::GetScenario, py::return_value_policy::reference);

	////////////////////////////////////
	//// Exceptions                 ////
	////////////////////////////////////
	py::register_exception<ScenarioFailedException>(m, "ScenarioFailedError");
}

