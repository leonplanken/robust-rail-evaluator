from flask_restful import Resource
from flask import request, Response, current_app
import json

from pyTORS import ArriveAction, ExitAction, BeginMoveAction, EndMoveAction, SplitAction, CombineAction, WaitAction, ServiceAction, MoveAction


class Actions(Resource):

    def get(self) -> str:
        """
        Get all possible actions and return them in json

        :return: all possible actions in json response
        """

        actions = {}
        for idx, a in enumerate(current_app.engine.get_actions(current_app.state)):
            # get shunting units from action
            shunting_units = [a.get_shunting_unit()]
            #for uuid in a.shunting_unit_uuids:
            #    shunting_units.append(str(uuid))
            

            actions[idx] = {"type": a.__class__.__name__,
                            "trains": [str(su) for su in shunting_units],
                            "tracks": [str(t.id) for t in a.get_reserved_tracks()],
                            "track_names": [str(t.name) for t in a.get_reserved_tracks()],
                            "duration": a.get_duration(),
                            "employees": [str(e) for e in a.get_employees()],
                            "reward": 0}

            if isinstance(a, ServiceAction):
                actions[idx]["task"] = str(a.get_task())

        return Response(json.dumps(actions), mimetype='application/json')

    def put(self):
        """
        Perform an action
        """

        action_id = int(request.args["action"])
        action = current_app.engine.get_actions(current_app.state)[action_id]

        current_app.engine.apply_action(current_app.state, action)
