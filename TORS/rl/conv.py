from itertools import chain, repeat, islice
import numpy as np
from pyTORS import TrackPartType, BeginMoveAction, EndMoveAction
from gym import spaces

"""
A class to convert the TORS state to a data tuple
"""
class ITORSConvertor:
    def __init__(self, location, *args, **kwargs):
        self.lcation = location
    """
    Convert a TORS state object to a data object, e.g. a tuple or a custom data object
    """
    def convert_state(self, state):
        raise NotImplementedError("This method has not been implemented.")

    """
    Convert an action to a TORS Action object
    """
    def convert_action(self, action):
        raise NotImplementedError("This method has not been implemented.")
    
    """
    Get the observation space, must be a gym spaces object
    """
    def get_observation_space(self):
        raise NotImplementedError("This method has not been implemented.")
    
    """
    Get the action space, must be a gym spaces object
    """
    def get_action_space(self):
        raise NotImplementedError("This method has not been implemented.")


"""
An example class implementing the ITORSConvertor
"""
class TORSConverter(ITORSConvertor):
    def __init__(self, location, *args, **kwargs):
        super(TORSConverter, self).__init__(location, *args, **kwargs)
        self.init_tracks()
        self.init_sizes()
    
    def init_tracks(self):
        self.track_map = {}
        self.tracks = []
        for i, track in enumerate([track for track in self.location.track_parts if track.type == TrackPartType.RAILROAD]):
            self.track_map[track] = i
            self.tracks.append(track)

    def init_sizes(self):
        self.n_inc = 3
        self.n_out = 3
        self.n_trains = 3
        self.track_pos_size = len(self.tracks)
        self.train_size = 3
        self.su_size = 4 + 2*self.track_pos_size + self.n_trains * self.train_size
        self.incoming_size = 1 + self.track_pos_size + self.su_size
        self.outgoing_size = 1 + self.track_pos_size + self.su_size
        self.n_actions = 5 + 2*(self.n_trains-1) + len(self.tracks) # see convert_action for this count of actions

    def get_observation_space(self, state):
        conv = self.convert_state(state)
        return spaces.Box(low=0, high=1, shape=(len(conv),), dtype=np.float16)
    
    def get_action_space(self, state):
        return spaces.Discrete(self.n_inc + self.n_out + self.n_trains * self.n_actions)

    """
    Returns the action space dimension:
    Per Shunting Unit:
    Arrive      (n_inc)
    Exit        (n_out)
    BeginMove   (1)
    EndMove     (1) 
    Wait        (1)
    Setback     (1)
    Service     (1)     [Assume there is only one task type]
    Split       (n_trains-1)
    Combine     (n-trains-1)
    Move        (n_tracks)
    """
    def convert_action(self, state, action):
        if action < self.n_inc:
            incoming = sorted(state.incoming_trains, key=lambda inc: inc.time)
            return Arrive(incoming[action])
        action -= self.n_inc
        if action < self.n_out:
            outgoing = sorted(state.outgoing_trains, key=lambda out: out.time)
            su = outgoing.shunting_unit #TODO
            return Exit(outgoing[action], su)
        action -= self.n_out
        su = state.shunting_units[action // self.n_actions]
        action_ix = action % self.n_actions
        if action_ix == 0: return BeginMoveAction(su)
        elif action_ix == 1: return EndMoveAction(su)
        elif action_ix == 2: return WaitAction(su)
        elif action_ix == 3: return SetbackAction(su)
        elif action_ix == 4:
            facility = state.get_position(su).facilities[0]
            for tu in su.train_units:
                tasks = state.get_tasks_for_train(tu)
                if len(tasks) == 0: continue
                return ServiceAction(su, tasks[0], tu, facility)
            raise Exception("Executing service task, but no task to be executed.")
        action_ix -= 5
        if action_ix < self.n_trains-1:
            return SplitAction(su, action_ix)
        action_ix -= self.n_trains-1
        if action_ix < self.n_trains-1: 
            return CombineAction(su, state.shunting_units[action_ix])
        action_ix -= self.n_trains-1
        return MoveAction(su, self.tracks[action_ix])

    def convert_state(self, state):
        return tuple(chain(
            self.convert_incomings(state),
            self.convert_outgoings(state),
            self.convert_tracks(state),
            self.convert_shunting_units(state),
            self.convert_time(state, state.time))
        )
    
    def convert_incomings(self, state):
        return pad(chain.from_iterable((self.convert_incoming(state, inc) for inc in state.incoming_trains)), self.n_inc * self.incoming_size)
        
    def convert_outgoings(self, state):
        return pad(chain.from_iterable((self.convert_outgoing(state, out) for out in state.outgoing_trains)), self.n_out * self.outgoing_size)
    
    def convert_incoming(self, state, inc):
        return chain(
            self.convert_time(state, inc.time),
            self.convert_track_position(inc.parking_track),
            self.convert_trains(inc.shunting_unit.trains)
        )

    def convert_outgoing(self, state, out):
        return chain(
            self.convert_time(state, out.time),
            self.convert_track_position(out.parking_track),
            self.convert_trains(out.shunting_unit.trains)
        )

    def convert_su(self, state, su):
        return chain(
            [
            state.is_moving(su), 
            state.is_waiting(su),
            state.is_in_neutral(su),
            state.has_active_action(su)
            ],
            self.convert_track_position(state.get_position(su)),
            self.convert_track_position(state.get_previous(su)),
            chain.from_iterable(self.convert_trains(state, state.get_train_units_in_order(su)))
        )
    
    def convert_trains(self, trains):
        return pad(chain.from_iterable((self.convert_train(train) for train in trains)), self.n_trains * self.train_size)

    def convert_train(self, train):
        if train.type.display_name == 'SLT4':
            return np.array([1,0,0])
        elif train.type.display_name == 'SLT6':
            return np.array([0,1,0])
        elif train.type.display_name == 'SNG3':
            return np.array([0,0,1])
        return np.array([0,0,0])

    def convert_tracks(self, state):
        return chain.from_iterable((self.convert_track(state, track) for track in self.tracks))

    def convert_track(self, state, track):
        sus_list = state.get_shunting_units()
        sus = state.get_occupations(track)
        if len(sus) > 1:
            positions = [state.get_position_on_track(su)+0.01*i for i,su in enumerate(sus)]
            shunting_units = list(zip(*sorted(zip(positions, sus))))[1]
        return pad(chain.from_iterable(
            (get_index(len(sus_list), sus_list.index(su)) for su in sus),
            self.n_trains * self.n_trains))

    def convert_track_position(self, track):
        return get_index(len(self.tracks), self.track_map[track])

    def convert_time(self, state, time):
        return [(time - state.start_time) / (state.end_time - state.start_time)]
    
def get_index(n, i):
        a = np.zeros(n)
        a[i] = 1
        return a

def pad_infinite(iterable, padding=0):
   return chain(iterable, repeat(padding))

def pad(iterable, size, padding=0):
   return islice(pad_infinite(iterable, padding), size)