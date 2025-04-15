# Description TORS

This document provides some details about TORS and the input required to run the evaluation and other functions.

### Location

The shunting yard is modeled as consisting of several tracks. Every track is connected to a number of tracks on the A-side and on the B-side. Each track has a type: `Bumper`, `Railroad`, or `Switch`. A bumper is the end of a track and is only connected on the A-side or B-side to one other track. A railroad is a track that is connected to one track at both the A- and B-side. Railroads have a length, and trains may be parked on railroads (if allowed). There are several types of switches. The switch is a track splitting into two tracks. An `English Switch` connects both A-side tracks to both B-side tracks, or both B-side tracks to A-side tracks. A `Half English Switch` connects A-side-1 to both B-sides, but A-side-2 is connected only to B-side-2. An `Intersection` connects four tracks, but A-side-1 is only connected with B-side-2, and A-side-2 is only connected to B-side-1. 

The tracks also have a number of attributes:
* **length**: this number describes the length of the track in meters (only Railroad tracks) and determines the amount of trains that can be parked on a track at one moment in time. 

* **saw_movement_allowed**: only if this attribute is true, can `setback` (change the shunting unit's direction) operations be performed on this track. 

* **parking_allowed**: only if this attribute is true, is parking allowed on this track

 Trains are directed. Therefore, a train's position cannot only be described by its current track, but also by its previous track. For this same reason, when you run, for example, the location.`get_shortest_path` function, you need to provide four tracks as parameters: (`from_previous`, `from_track`, `to_previous`, `to_track`). 

When a train arrives, the incoming object has a `parking_track` attribute and a side_track attribute. The latter describes the "`bumper`" from which the shunting unit will enter the yard. Once it has arrived, this side track becomes the previous track of the shunting unit. For the outgoing shunting unit, the side_track describes the "`bumper`" over which the shunting unit will depart from the yard.

### Shunting units 

In TORS, the most important entities are shunting units. A shunting unit is a combination of one or more trains that is shunted as one unit. In this terminology (unlike what you might expect) a train is considered to be an atomic unit (even though a train exists of several carriages), i.e., in TORS a train cannot be split. The number of trains and shunting units is therefore not necessarily the same. Note also that this means that trains are atomic and persistent, whereas shunting units are transient! 

Every train has a unique id. The shunting units also have id's, which are unique on a given moment in time, but over time the same id can be used for different shunting units. For example, a scenario may have an incoming shunting unit with id 33333, consisting of two trains with ids 2401 and 2402, and an outgoing shunting unit also with id 33333, but with one train with id 2401. 

In the first phase of the competition, only shunting units consisting of one train are considered, so this can still be ignored. But for later phases the distinction between a shunting unit and a train will become important. 

Also, in the second phase, the matching problem will be added to the challenge. This means that outgoing goals will not be specified by train ids, but by train types, e.g. a shunting unit with id 11111 should leave from track 1 at time 800 consisting of two trains of type SLT4 (instead of two trains with id 2401 and 2402).

Every train has a type. In phase 1, the train type influences the problem in two ways:

* The train type determines the length of the setback action. Therefore, the location.get_shortest_path function also requires the train type as a parameter. 

* The train type determines the length of a shunting unit. This is important, because fewer longer trains can fit on one track than short trains.

In summary: 

* Trains are atomic and persistent. Shunting units are transient and can be split and joined together. 

* Train ids are unique. Shunting unit ids are only unique on that moment in time, but may be reused later in time. 

* Incoming and outgoing goals are defined on shunting units. 

* Maintenance tasks are defined for trains.

### TORS simulation loop 

TORS is a state-event-action based simulator and operates as follows: 

1) An initial state is generated from the scenario description. 

2) **While** the scenario is not finished (i.e. there are remaining events or the scenario end time is not reached yet): 

    1) Execute all events that are triggered on this timestamp. 

    2) If no action is required (i.e. all shunting units have an active action), go back to 2.1 

    3) Read the next action from the plan.

        1) Execute the action. (if invalid, stop the scenario) 

