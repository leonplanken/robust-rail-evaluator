import gym
import importlib
from gym import spaces
from rl.conv import TORSConverter
from pyTORS import Engine, Action

class TORSEnv(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self, config):
        super(TORSEnv, self).__init__()
        self.config = config
        self.engine = Engine(config['data folder']) 
        self.location = self.engine.get_location()
        self.org_scenario = self.engine.get_scenario()
        self.scenario_generator = self._get_generator(1)
        self.scenario_generator.initialize(self.org_scenario, self.location)
        self.state = None
        self.scenario = None
        self.converter = self._get_converter()
        self.reset()
        self.action_space = self.converter.get_action_space(self.state)
        self.observation_space = self.converter.get_observation_space(self.state)

    def step(self, action):
        reward = 0
        done = False
        info = {}
        if not isinstance(action, Action):
            action = self.converter.convert_action(self.state, action)
        try:
            self.state = self.engine.apply_action(self.state, action)
        except ScenarioFailedError:
            reward = 0
            done = True
        except InvalidActionError:
            reward = -1
            done = True
        if not done:
            if self.state.end_time == self.state.time and self.state.peek_event() is None:
                done = True
                reward = 1 if len(self.state.incoming_trains) == 0 and len(self.state.outgoing_trains) == 0 else 0
        return self.converter.convert_state(self.state), reward, done, info
        
    def reset(self):
        self._reset()
        return self.converter.convert_state(self.state)
        
    def render(self, mode='human'):
        pass

    def close (self):
        if not self.state is None:
            self.engine.end_session(self.state)


    def _reset(self):
        if not self.state is None:
            self.print("TE> End previous session")
            self.engine.end_session(self.state)
        self.print("TE> Delete scenario")
        del self.scenario

        # TODO: Shunting units are stored in incoming and outgoin goals, which are deleted when the scenario is deleted,
        # and also in the state, which is deleted when the state is deleted.

        self.print("TE> Generate scenario")
        self.scenario = self.scenario_generator.generate_scenario()
        self.print("TE> Start new session")
        self.state = self.engine.start_session(self.scenario)
        self.print("TE> Started new session")
        self.result = 0

    def _get_generator(self, n_trains):
        generator_str = self.config.generator['class']
        generator_lst = generator_str.split('.')
        _module = importlib.import_module(".".join(generator_lst[:-1]))
        _class = getattr(_module, generator_lst[-1])
        config = self.config.generator.copy()
        del config['class']
        if generator_str in self.config:
            config.update(self.config[generator_str])
        return _class(n_trains=n_trains, **config)

    def _get_converter(self):
        converter_str = self.config.converter['class']
        converter_lst = converter_str.split('.')
        _module = importlib.import_module(".".join(converter_lst[:-1]))
        _class = getattr(_module, converter_lst[-1])
        config = self.config.converter.copy()
        del config['class']
        if converter_str in self.config:
            config.update(self.config[converter_str])
        return _class(self.location, **config)
	
    def print(self, text):
    	print(text)
