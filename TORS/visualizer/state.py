from flask_restful import Resource
from flask import Response, current_app
import json


class State(Resource):

    def get(self) -> str:
        """
        Get current state and put in json

        :return: Part of the state in a json response
        """

        coordinates = current_app.vis_config["coordinates"]
        scale = current_app.vis_config["scale"]
        offset_x = current_app.vis_config["offset_x"]
        offset_y = current_app.vis_config["offset_y"]

        tracks = {}
        state = current_app.state
        # get tracks
        for track in current_app.engine.get_location().get_track_parts():
            id = track.id
            tracks[id] = []

            # get trains on track
            for train in state.get_occupations(track):
                if state.get_position(train) == track:
                    # get direction
                    direction = None
                    previous = state.get_previous(train)
                    if previous and str(previous.id) in coordinates:
                        # direction track is connected to current track (point in common)
                        if coordinates[str(id)][0] in coordinates[str(previous.id)]:
                            direction = coordinates[str(id)][0]
                        else:
                            direction = coordinates[str(id)][-1]

                        direction = [direction[0] * scale + offset_x,
                                     direction[1] * scale + offset_y]
                    in_neutral = state.is_in_neutral(train)
                    moving = state.is_moving(train)
                    train_obj = {
                        "id": train.id,
                        "unique_id": str(train.id),
                        "direction": direction,
                        "in_neutral": in_neutral,
                        "moving": moving,
                        "train_units": [tu.id for tu in train.get_trains()]
                    }

                    tracks[id].append(train_obj)

        # get reserved tracks
        reserved_tracks = []#state.get_reserved_tracks()

        # get next event
        if state.peek_event():
            next_event = str(state.peek_event().get_type())
        else:
            next_event = "Nothing"

        state = {
            "time": state.time,
            "tracks": tracks,
            "reserved_tracks": reserved_tracks,
            "next_event": next_event
        }

        return Response(json.dumps(state), mimetype='application/json')
