import json
from typing import List, Dict

class DynamicJSONGenerator:
    def __init__(self, json_file: str = None):
        """
        Initializes the generator with an optional JSON file. 
        If a file is provided, it will be loaded.
        """
        # Default structure with all required keys and subkeys
        self.default_structure = {
            "trackParts": [],
            "facilities": [],
            "taskTypes": [],
            "movementConstant": 0,
            "movementTrackCoefficient": 60,
            "movementSwitchCoefficient": 30,
            "distanceEntries": []
        }

        # Default substructures
        self.default_track_part = {
            "id": "0",
            "type": "RailRoad",
            "aSide": [],
            "bSide": [],
            "length": 0,
            "name": "",
            "sawMovementAllowed": False,
            "parkingAllowed": False,
            "isElectrified": True,
        }

        self.default_facility = {
            "id": "",
            "type": "",
            "relatedTrackParts": [],
            "taskTypes": [],
            "simultaneousUsageCount": 0,
            "timeWindow": {"start": 0, "end": 0}
        }

        self.default_distance_entry = {
            # "fromTrackPartId": "",
            # "toTrackPartId": "",
            # "distanceInSeconds": 0
        }

        # Load data from file or use an empty default structure
        self.data = {}
        if json_file:
            self.load_json(json_file)
        else:
            self.data = self.default_structure

    def load_json(self, json_file: str):
        """Loads data from an existing JSON file."""
        try:
            with open(json_file, 'r') as file:
                self.data = json.load(file)
            print(f"Loaded JSON data from {json_file}")
            self.validate_and_update_structure()  # Validate and fix structure after loading
        except FileNotFoundError:
            print(f"File {json_file} not found. Starting with a default structure.")
            self.data = self.default_structure
        except json.JSONDecodeError:
            print(f"Error decoding JSON from {json_file}. Starting with a default structure.")
            self.data = self.default_structure

    def save_json(self, json_file: str):
        """Saves the current data to a JSON file."""
        with open(json_file, 'w') as file:
            json.dump(self.data, file, indent="\t")
        print(f"Saved changes to {json_file}")

    def validate_and_update_structure(self):
        """Validates and updates the JSON structure with missing keys and default values."""
        # Ensure main keys exist in the input file, and if not, add them
        for key, default_value in self.default_structure.items():
            if key not in self.data:
                self.data[key] = default_value

        # Ensure 'trackParts' are updated if they exist
        if "trackParts" in self.data:
            self.data["trackParts"] = [
                {**self.default_track_part, **track_part}  # Only update missing keys, keep existing structure
                for track_part in self.data["trackParts"]
            ]

        # Ensure 'facilities' are updated if they exist
        if "facilities" in self.data:
            self.data["facilities"] = [
                {**self.default_facility, **facility}  # Only update missing keys, keep existing structure
                for facility in self.data["facilities"]
            ]

        # Ensure 'distanceEntries' are updated if they exist
        if "distanceEntries" in self.data:
            self.data["distanceEntries"] = [
                {**self.default_distance_entry, **distance_entry}  # Only update missing keys, keep existing structure
                for distance_entry in self.data["distanceEntries"]
            ]
        
        for track in self.data["trackParts"]:
            if track["name"] == "Spoor906a":
                track["length"] = 600


    def add_track_part(self, **kwargs):
        """Adds a track part to the data."""
        track_part = {**self.default_track_part, **kwargs}
        self.data["trackParts"].append(track_part)

    def add_facility(self, **kwargs):
        """Adds a facility to the data."""
        facility = {**self.default_facility, **kwargs}
        self.data["facilities"].append(facility)

    def add_distance_entry(self, **kwargs):
        """Adds a distance entry to the data."""
        distance_entry = {**self.default_distance_entry, **kwargs}
        self.data["distanceEntries"].append(distance_entry)

    def set_movement_constants(self, constant: int, track_coefficient: int, switch_coefficient: int):
        """Sets the movement constants."""
        self.data["movementConstant"] = constant
        self.data["movementTrackCoefficient"] = track_coefficient
        self.data["movementSwitchCoefficient"] = switch_coefficient

    def to_json(self) -> str:
        """Returns the JSON data as a string."""
        return json.dumps(self.data, indent=4)


# Example Usage
if __name__ == "__main__":
    json_file = "input_location.json"  # Specify the input file name
    output_file = "location_output.json"  # Specify the output file name

    # Initialize with an existing JSON file or an empty structure
    generator = DynamicJSONGenerator(json_file=json_file)

    # Save the modified data back to the output JSON file (keeping input file intact)
    generator.save_json(output_file)
