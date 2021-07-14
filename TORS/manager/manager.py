from manager.simulator import Simulator
import importlib
import threading
import os
from time import time
#from planner.planner import RemotePlanner

class Manager:
    
    def __init__(self, episode_config, agent_config):
        self.episode_config = episode_config
        self.agent_config = agent_config
        self.simulator = Simulator(episode_config)
        self.planner = self.get_planner()
        self.print_episode_info()
        self.print_agent_info()
    
    def print_episode_info(self):
        self.print("M> ### Episode info ###")
        self.print("M> Data folder: {}".format(self.episode_config['data folder']))
        self.print("M> Scenario(s): {}".format(self.episode_config['scenario']))
        self.print("M> Number of runs: {}".format(self.episode_config['n_runs']))
        self.print("M> Maximum number of trains: {}".format(self.episode_config['max_trains']))
    	
    def print_agent_info(self):
        self.print("M> ### Agent info ###")
        planner_class = self.agent_config.planner['class']
        self.print("M> Agent class: {}".format(planner_class))
        if planner_class in self.agent_config:
            config = self.agent_config[planner_class]
            for key, val in config.items():
                self.print("M> \t{}: {}".format(key, val))
      
    def run(self):
        self.sol_runtime = 0
        self.sim_runtime = 0
        self.gen_runtime = 0
        
        sim_start = time()
        self.simulator.start()
        self.sim_runtime += time() - sim_start

        start = time()
        self.planner.set_location(self.simulator.get_location())
        self.sol_runtime += time() - start

        results = {}
        fails = {}
        for t in range(1, self.simulator.get_max_trains()+1):
            results[t] = 0
            fails[t] = 0
            
            gen_start = time()
            self.simulator.set_n_trains(t)
            self.gen_runtime += time() - gen_start
            
            self.print("M> ###############################")
            self.print("M> ### Testing with {:3} trains ###".format(t))
            self.print("M> ###############################")
            for r in range(self.episode_config.n_runs):
                self.print("M> ### Run {:3}                 ###".format(r+1))
                result, failure = self.run_one()
                results[t] += result
                fails[t] += failure
                if failure: self.print("M> Scenario failed")
                else: self.print("M> Result: {}".format(result))
            self.print("M> Average score: {}%, Failures: {}%".format(results[t]/self.episode_config.n_runs * 100., fails[t]/self.episode_config.n_runs * 100))
            if fails[t] > 0 or results[t] == 0: break
        for t in results:
            self.print("M> {} Trains |\tAverage score: {}%, Failures: {}%".format(t, results[t]/self.episode_config.n_runs * 100., fails[t]/self.episode_config.n_runs * 100))
        print("M> Time spent in solver: {}s".format(self.sol_runtime))
        print("M> Time spent in simulator: {}s".format(self.sim_runtime))
        print("M> Time spent in scenario generator: {}s".format(self.gen_runtime))
        print("M> Total accounted time: {}s".format(self.sol_runtime + self.sim_runtime + self.gen_runtime))    
    def run_one(self):
        failure = 0
        self.simulator.reset()
        start = time()
        self.planner.reset()
        self.sol_runtime += time() - start
        while True:
            sim_start = time()
            state, actions = self.simulator.get_state()
            self.sim_runtime += time() - sim_start
            if not actions: break
            try:
                start = time()
                action = self.planner.get_action(state, actions)
                self.sol_runtime += time() - start
                sim_start = time()
                if not self.simulator.apply_action(action): break
                self.sim_runtime += time() - sim_start
            except Exception as e:
                print(e)
                raise e
                failure = 1
                break
        result = self.simulator.get_result()
        start = time()
        self.planner.report_result(result)
        self.sol_runtime += time() - start
        return result, failure
     
    def print(self, m):
        if self.episode_config.verbose >= 1: print(m)   
                
    def get_planner(self):
        planner_str = self.agent_config.planner['class']
        planner_lst = planner_str.split('.')
        _module = importlib.import_module(".".join(planner_lst[:-1]))
        _class = getattr(_module, planner_lst[-1])
        if planner_str in self.agent_config:
            config = self.agent_config[planner_str]
        else: config = {} 
        planner = _class(self.agent_config.planner.seed, self.agent_config.planner.verbose, config)
        #if self.agent_config.planner.remote:
        #    planner = RemotePlanner(planner, self.agent_config)
        return planner
    
