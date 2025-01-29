import json
from typing import List, Dict, Any
import copy



def sort_actions(actions):
        """Sorts the list of actions based on suggestedStartingTime and duration."""
    
        def compare(action1, action2):
            """Comparison function for sorting."""
            start1 = int(action1.get("suggestedStartingTime", 0))
            start2 = int(action2.get("suggestedStartingTime", 0))
            finish1 = int(action1.get("suggestedFinishingTime", start1))
            finish2 = int(action2.get("suggestedFinishingTime", start2))

            # Compute durations
            duration1 = finish1 - start1
            duration2 = finish2 - start2

            # Primary sort: suggestedStartingTime
            if start1 != start2:
                return start1 - start2

            # Secondary sort: duration (if same starting time)
            return duration1 - duration2
          
    # Implementing Bubble Sort (or any sorting algorithm)
        n = len(actions)
        for i in range(n):
            for j in range(0, n - i - 1):
                if compare(actions[j], actions[j + 1]) > 0:
                    actions[j], actions[j + 1] = actions[j + 1], actions[j]  # Swap
    
        return actions



class JSONGenerator:
    def __init__(self, input_json_file: str):
        """
        Initialize the generator with an input JSON file.
        """
        self.input_file = input_json_file
        self.input_data = self.load_json(input_json_file)

        self.data = {
            "location": "",
            "scenario": {},
            "plan": {},
        }
        


        self.trainUnits_total = []
        self.times = []


    def load_json(self, json_file: str) -> Dict[str, Any]:
        """
        Load the JSON file.
        """
        try:
            with open(json_file, 'r') as file:
                return json.load(file)
        except (FileNotFoundError, json.JSONDecodeError) as e:
            print(f"Error loading JSON file: {e}")
            return {}

    def save_json(self, filename: str):
        """Saves the current structure to a JSON file."""
        with open(filename, 'w') as file:
            json.dump(self.data, file, indent="\t")

    

    def createActions_inputJSON(self):

        # Keep storing the actions the train uints did
        # it helps to know the last action when a new one is appended
        # if the new action is a movement, the previous action must be checked
        # if it was not a movement aciton as well, if not a MoveBegin action shoudl
        # also be initiated.
        # Look Up Table {"trainUintID": List[actions]}
        allTrainUnits = {}

        self.data["plan"].update({"actions":[]})


        for action in self.input_data["actions"]:

            trainUnit = action["shuntingUnit"]
            _trainUnitIds = []

            for shuntUnit in trainUnit["members"]:
                _trainUnitIds.append(shuntUnit["id"])
                if shuntUnit["id"] not in allTrainUnits:
                    allTrainUnits.update({shuntUnit["id"]:[]})

            if (int(action["endTime"]) - int(action["startTime"])) == 0:
                _actions = {
                       "suggestedStartingTime": action["startTime"],
                       "suggestedFinishingTime": action["endTime"],
                       "trainUnitIds": _trainUnitIds
                    }
                # self.data["plan"]["action"].append(_actions)
            else:
                _actions = {
                       "suggestedStartingTime": action["startTime"],
                       "suggestedFinishingTime": action["endTime"],
                       "minimumDuration" : "" + str(int(action["endTime"]) - int(action["startTime"])),
                       "trainUnitIds": _trainUnitIds
                    }

            #TODO test action if movement or tassk which is present

            # Move, Split, Combine, Arrive, Exit, Wait
            if "predefined" in action["taskType"]:
                specific_action = action["taskType"]["predefined"]
                if specific_action == "Move":
                    moves = []
                    #current location
                    moves.append(action["location"])
                    for move_location in action["resources"]:
                        moves.append(move_location["trackPartId"])


                    _actions.update({"movement":{"path":moves}
                    })

                elif specific_action == "Exit":
                    _actions.update({"task":{"type":{"predefined":"Exit"}}
                    })

                elif specific_action == "Arrive":
                    _actions.update({"task":{"type":{"predefined":"Arrive"}}
                    })

                elif specific_action == "Split":
                    _actions.update({"task":{"type":{"predefined":"Split"}, "trainUnitIds":[_actions["trainUnitIds"][0]]}
                    })
                    
                elif specific_action == "Combine":
                    _actions.update({"task":{"type":{"predefined":"Combine"}, "trainUnitIds":[_actions["trainUnitIds"]]}
                    })    
                    
                elif specific_action == "Wait":
                    _actions.update({"break":{}})  


            # Name of the servicemovement
            elif "other" in action["taskType"]:
                specific_action = action["taskType"]["other"]
                
                facilityIds = []
                for resource in action["resources"]:
                    facilityIds.append({"id":resource["facilityId"]})
                
                
                _actions.update({"task":{"type":{"other": specific_action},
                                         "location":  action["location"],
                                         "facilities": facilityIds, 
                                         "trainUnitIds":[_actions["trainUnitIds"]]}
                    })
            else:
                specific_action = ""
                print("Error while parsing taskType")




            # Returns the action space dimension:
            # Per Shunting Unit:
            # Arrive      (n_inc)
            # Exit        (n_out)
            # BeginMove   (1)
            # EndMove     (1)
            # Wait        (1)
            # Setback     (1)
            # Service     (1)     [Assume there is only one task type]
            # Split       (n_trains-1)
            # Combine     (n-trains-1)
            # Move        (n_tracks)


            # // Task represents a shunting unit moving from one location to another location.
            # Move = 0;
            # // Task represents a shunting unit splitting into multiple shunting units.
            # Split = 1;
            # // Task represents two shunting units combining into one shunting unit.
            # Combine = 2;

            # // Task represents a shunting unit being parked on a track.
            # Wait = 3;
            # // Task represents the arrival of a shunting unit on the facility.
            # Arrive = 4;
            # // Task represents the departure of a shunting unit on the facility.
            # Exit = 5;
            # // Task represents the reversal of a shunting unit.
            # Walking = 6;
            # // Task represents the break of a member of staff.
            # Break = 7;
            # // Non-Service traffic that blocks the infrastructure
            # NonService = 8;

            # BeginMove = 9;//TEMP

            # EndMove = 10;//TEMP




            self.data["plan"]["actions"].append(_actions)
        
        
        # print(self.data["plan"]["actions"])
        
        sorted_actions = copy.deepcopy(sort_actions(self.data["plan"]["actions"]))
        # print(sorted_actions)
        
        self.data["plan"]["actions"].clear()
        
        
        
        for _sorted_action in sorted_actions:
            self.data["plan"]["actions"].append(_sorted_action)
        
        
        alpha_actions = copy.deepcopy(self.data["plan"]["actions"])
        
    
            
        # iterate trough all the actions declared previously to find if 
        # there are BeginMove and EndMove to be added before Move actions
        # BeginMove and EndMove are needed when before the precedent action of 
        # a Move action is different then a Move action, it is a requirement of cTORS
        globalCounter = 0
        beta_actions = self.data["plan"]["actions"]
        for index, beta_action in enumerate(beta_actions):
            if "movement" in beta_action:
                trainUnitIds = beta_action["trainUnitIds"]
                trainUnitInAction = {}
                trainUintActions =[]
                for trainUint in trainUnitIds:
                    trainUnitInAction.update({trainUint: trainUintActions})
                    for precedent_beta_action in beta_actions[:index]:
                        if trainUint in precedent_beta_action["trainUnitIds"]:
                            if "movement" in precedent_beta_action:
                                trainUintActions.append("movement")
                            elif "task" in precedent_beta_action:
                                if "predefined" in precedent_beta_action["task"]["type"]:
                                    trainUintActions.append(precedent_beta_action["task"]["type"]["predefined"])
                                else:
                                    trainUintActions.append(precedent_beta_action["task"]["type"]["other"])
                            elif "break" in precedent_beta_action:
                                trainUintActions.append("break")
                         
                                
                            trainUnitInAction.update({trainUint: trainUintActions})
            
                setOftrainUnitActions = list(trainUnitInAction.values())
                # Check if all lists are identical
                if all(l == setOftrainUnitActions[0] for l in setOftrainUnitActions):
                    trainUnitActions = setOftrainUnitActions[0]
                    if trainUnitActions:
                        if trainUnitActions[-1] == "movement" and trainUnitActions[-1] != "Arrive":
                            print("-> movement")
                            _actions = {
                                "suggestedStartingTime": beta_action["suggestedStartingTime"],
                                "suggestedFinishingTime": beta_action["suggestedStartingTime"],
                                "trainUnitIds": beta_action["trainUnitIds"],
                                "task": { "type": { "predefined": "BeginMove"}}
                            }
                            alpha_actions.insert(index-1+globalCounter, _actions)
                            globalCounter = globalCounter + 1
                            
                        if trainUnitActions[-1] == "Arrive":
                            _actions = {
                                "suggestedStartingTime": beta_action["suggestedStartingTime"],
                                "suggestedFinishingTime": beta_action["suggestedStartingTime"],
                                "trainUnitIds": beta_action["trainUnitIds"],
                                "task": { "type": { "predefined": "BeginMove"}}
                            }
                            alpha_actions.insert(index+globalCounter, _actions)
                            globalCounter = globalCounter + 1

                else:    
                    for trainUint, trainUnitActions in trainUnitInAction.items():
                        if trainUnitActions:
                            # if the last action before the current actions of the given train unit is 
                            # not a movement a BeginMove action has to be added to the actions
                            if trainUnitActions[-1] != "movement":
                                print("Iron Man")
        
        
        alpha_actions_ext = copy.deepcopy(alpha_actions)
        globalCounter_ext = 0
        for index, _alpha_action in enumerate(alpha_actions):
            contains = False
            if "taks" not in _alpha_action and "movement" in _alpha_action:
                previous_action = alpha_actions[index-1]
                # print(previous_action)
                # print(previous_action["task"]["type"]["predefined"])
               
               
               
               # add breake
               
                if "task" in previous_action:  
                    # print(previous_action)
                    if "predefined" in previous_action["task"]["type"]:
                        if  "BeginMove" != previous_action["task"]["type"]["predefined"]:
                            _actions = {
                                           "suggestedStartingTime": _alpha_action["suggestedStartingTime"],
                                           "suggestedFinishingTime": _alpha_action["suggestedStartingTime"],
                                           "trainUnitIds": _alpha_action["trainUnitIds"],
                                           "task": { "type": { "predefined": "BeginMove"}}
                                       }
                            alpha_actions_ext.insert(index+globalCounter_ext, _actions)
                            globalCounter_ext = globalCounter_ext + 1
                    elif "other" in previous_action["task"]["type"]:
                        # print(_alpha_action)
                        # print(previous_action)
                        _actions = {
                                           "suggestedStartingTime": _alpha_action["suggestedStartingTime"],
                                           "suggestedFinishingTime": _alpha_action["suggestedStartingTime"],
                                           "trainUnitIds": _alpha_action["trainUnitIds"],
                                           "task": { "type": { "predefined": "BeginMove"}}
                                       }
                        alpha_actions_ext.insert(index+globalCounter_ext, _actions)
                        globalCounter_ext = globalCounter_ext + 1       
                else:
                    # print(previous_action)
                    _actions = {
                                   "suggestedStartingTime": _alpha_action["suggestedStartingTime"],
                                   "suggestedFinishingTime": _alpha_action["suggestedStartingTime"],
                                   "trainUnitIds": _alpha_action["trainUnitIds"],
                                   "task": { "type": { "predefined": "BeginMove"}}
                               }
                    alpha_actions_ext.insert(index+globalCounter_ext, _actions)
                    globalCounter_ext = globalCounter_ext + 1
        
        self.data["plan"]["actions"].clear()
        
        for _alpha_action in alpha_actions_ext:
            self.data["plan"]["actions"].append(_alpha_action)
        
        alpha_actions_endtime = copy.deepcopy(alpha_actions_ext)
        globalCounter_endTime = 0
        for index, _alpha_action in enumerate(alpha_actions_ext):
            if "taks" not in _alpha_action and "movement" in _alpha_action:
                feseable = False
                next_index = index
                while feseable is not True:                        
                    next_action = alpha_actions_ext[next_index]        
                    if int(next_action["suggestedStartingTime"]) == int(_alpha_action["suggestedFinishingTime"]):
                        feseable = True
                        print(f'{_alpha_action} --- EndMove --- \n {next_action}')
                        _actions = {
                                "suggestedStartingTime": next_action["suggestedStartingTime"],
                                "suggestedFinishingTime": next_action["suggestedStartingTime"],
                                "trainUnitIds": next_action["trainUnitIds"],
                                "task": { "type": { "predefined": "EndMove"}}
                            }
                        alpha_actions_endtime.insert(next_index+globalCounter_endTime, _actions)      
                        globalCounter_endTime = globalCounter_endTime + 1  
                    next_index = next_index + 1
                    if next_index >= len(alpha_actions_ext):
                        feseable = True
                
                #Check when next action can be inserted 
                # when another action's suggestedStartingTime is equal to suggestedFinishingTime of the movement action 
        
        self.data["plan"]["actions"].clear()
        
        for _alpha_action in alpha_actions_endtime:
            self.data["plan"]["actions"].append(_alpha_action)
        # for _alpha_action in alpha_actions:
        #     self.data["plan"]["actions"].append(_alpha_action)
                                
                                

if __name__ == "__main__":
    input_file = "plan_input.json"  # Path to the input JSON file
    output_file = "pland_output.json"  # Path to save the output JSON file

    generator = JSONGenerator(input_json_file=input_file)
    generator.createActions_inputJSON()

    generator.save_json("generated_hip_plan.json")
