from abc import ABC, abstractmethod
import random

class Planner(ABC):
    
    def __init__(self, seed, verbose, time_limit, config):
        """
        Initialize the planner with a random seed, a verbose setting and a time limit.
        The time limit is just for info. The planner is stopped after this limit. 
        The config parameter contains the contents of the agent config file (default agent.json)
        """
        super(Planner, self).__init__()
        self.random = random.Random(seed)
        self.location = None
        self.verbose = verbose
        self.config = config
        self.time_limit = time_limit
    
    def get_random(self):
        """
        Get the random generator initialized with the given seed
        """
        return self.random
    
    def set_location(self, location):
        """
        Set the location of the planner
        """
        self.location = location
        
    def get_location(self):
        """
        Get the location
        """
        return self.location
    
    def report_result(self, result):
        """
        Report the scenario result back to the planner
        """
    
    @abstractmethod
    def get_action(self, state, actions): # TODO remove actions parameter
        """
        Get an action for the given state, and choose from one of the actions in the list actions
        """
    
    @abstractmethod
    def reset(self):
        """
        Reset the planner. This method is called after every run
        """

    @abstractmethod
    def close(self):
        """
        Close the planner and close all resources.
        This method is called at the end of all runs
        """
    
    def print(self, m):
        if self.verbose >= 1: print(m)
