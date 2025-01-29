import json
from typing import List, Dict, Any


class JSONGenerator:
    def __init__(self, input_json_file: str):
        """
        Initialize the generator with an input JSON file.
        """
        self.input_file = input_json_file
        self.input_data = self.load_json(input_json_file)

        self.data = {
            "nonServiceTraffic": [],
            "disabledTrackPart": [],
            "workers": [],
            "in": [],
            "out": [],
            "startTime": "0",
            "endTime": "1500",
            "inStanding": [],
            "outStanding": [],
            "trainUnitTypes": []
        }
        
        self.trainUnits_total = []
        self.times = []
      
    def add_entry(self, group: str, time: str, entry_id: str, side_track_part: str, parking_track_part: str, members: List[Dict[str, Any]]):
        """ Adds an entry to either 'in' or 'out'."""
        entry = {
            "time": time,
            "id": entry_id,
            "sideTrackPart": side_track_part,
            "parkingTrackPart": parking_track_part,
            "members": members
        }
        if group in self.data:
            self.data[group].append(entry)
      
    
    def create_member(self, member_id: str, type_display_name: str, tasks: List[Dict[str, Any]] = None) -> Dict[str, Any]:
        """ Creates a member dictionary."""
        if tasks is None:
            tasks = []
        return {
            "id": member_id,
            "typeDisplayName": type_display_name,
            "tasks": tasks
        }
      
      
      
      

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

    def getInTrains_inputJSON(self):
        group = "in"
        # Each train corresponds to one set under the "in" key
        for train in self.input_data["in"]["trains"]: 
            members = []
            
            # Look for each member 
            for member in train["members"]:
                tasks = []
                # Look for each task in a given member
                for task in member["tasks"]:
                    tasks.append({
                        "type": {"other": task["type"]["other"]},
                        "priority": 0,
                        "duration": task["duration"],
                        "requiredSkills": task["type"]["other"]
                    })
                
                trainUint = member["trainUnit"]
                members.append({
                    "id": trainUint["id"],
                    "typeDisplayName": trainUint["type"]["displayName"] + str(trainUint["type"]["carriages"]),
                    "tasks": tasks})
                
                self.trainUnits_total.append(trainUint)
            # trainUnitTypes 
            
            entry = {
            "time": train["arrival"],
            "id": train["id"],
            "sideTrackPart": train["entryTrackPart"],
            "parkingTrackPart": train["firstParkingTrackPart"],
            "members": members
            }
            self.times.append(entry["time"])
            
            if group in self.data:
                self.data[group].append(entry)
        
        group = "out"
        
        for train in self.input_data["out"]["trainRequests"]: 
            trainUnits = []
            for trainUnit in train["trainUnits"]:
                trainUnitType = trainUnit["type"]
                trainUnits.append({
                    "id" : "****",
                    "typeDisplayName": trainUnitType["displayName"] + str(trainUnitType["carriages"]),
                    "tasks": []
                })
                self.trainUnits_total.append(trainUint)
                
            entry = {
                "time": train["arrival"],
                "id": train["displayName"],
                "sideTrackPart": train["leaveTrackPart"],
                "parkingTrackPart": train["lastParkingTrackPart"],
                "members": trainUnits
            }
            self.times.append(entry["time"])

            if group in self.data:
                self.data[group].append(entry)
                          
        
    def setStartEndTimes(self):
        int_times = []
        for t in self.times:
            int_times.append(int(t))        
        
        self.data["startTime"] = min(int_times)
        self.data["endTime"] = max(int_times)        

    def getTrainUnitTypes(self):
        
        for trainUint in self.trainUnits_total:        
            trainUintType = trainUint["type"]
            train_unit = {
                "displayName": trainUintType["displayName"] + str(trainUintType["carriages"]),
                "carriages": trainUintType["carriages"],
                "length": trainUintType["length"],
                "combineDuration": trainUintType["combineDuration"],
                "splitDuration": trainUintType["splitDuration"],
                "backNormTime": trainUintType["backNormTime"],
                "backAdditionTime": trainUintType["backAdditionTime"],
                "travelSpeed": "0",
                "startUpTime": "0",
                "typePrefix": trainUintType["displayName"],
                "needsLoco": False,
                "isLoco": False,
                "needsElectricity": True
            }
            
            # Ensures that the same trainUnitType is appended only once
            exist = False
            for existing_train_unit in self.data["trainUnitTypes"]:
                #trainUnitTypes[] already contains the same trainUnitType 
                if train_unit["displayName"] == existing_train_unit["displayName"]:
                    exist = True
            
            # trainUnitType was not yet added to the list      
            if exist == False:         
                self.data["trainUnitTypes"].append(train_unit)

# Example Usage
if __name__ == "__main__":
    input_file = "scenario_input.json"  # Path to the input JSON file
    output_file = "scenario_output.json"  # Path to save the output JSON file

    generator = JSONGenerator(input_json_file=input_file)

    # member1 = generator.create_member("2422", "SLT4", [])
    # member2 = generator.create_member("2433", "SLT4", [])
    # generator.add_entry("in", "30", "11111", "14", "1", [member1, member2])
    
    generator.save_json("generated_structure.json")
    
    generator.getInTrains_inputJSON()
    generator.getTrainUnitTypes()
    generator.setStartEndTimes()
    
    generator.save_json("generated_main_cTORS.json")
    