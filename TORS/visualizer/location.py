import svgwrite
from flask_restful import Resource
from flask import Response, current_app
from pyTORS import Facility
from pyTORS import Location
from pyTORS import RAILROAD
from pyTORS import BUMPER
from pyTORS import Track
from typing import List


class Location(Resource):

    def get(self) -> str:
        """
        Generate the svg and return it

        :return: Location svg
        """

        return Response(self.generate_svg(current_app.engine.get_location()),
                        mimetype='image/svg+xml')

    def generate_svg(self, location: Location) -> str:
        """
        Generate the svg from the location object

        :param location: the location
        :return: svg of the location
        """

        track_scale = current_app.vis_config["scale"]
        offset_x = current_app.vis_config["offset_x"]
        offset_y = current_app.vis_config["offset_y"]

        width = str(current_app.vis_config["width"]) + "px"
        height = str(current_app.vis_config["height"]) + "px"
        svg_document = svgwrite.Drawing("location.svg", size=(width, height))

        # Draw the base
        for track in location.get_track_parts():
            id = track.id
            coordinates = current_app.vis_config["coordinates"]

            if str(id) in coordinates:
                # scale
                scaled = [[x[0] * track_scale + offset_x,
                           x[1] * track_scale + offset_y] for x in coordinates[str(id)]]

                self.draw_tracks(svg_document, scaled, track)

        # Draw the facilities
        for facility in location.get_facilities():

            if str(facility.id) in current_app.vis_config["facilities"]:
                config = current_app.vis_config
                for track in facility.get_tracks():
                    track_coordinates = \
                        config["coordinates"][str(track.id)]

                    # scale
                    coordinates = [[x[0] * track_scale + offset_x,
                                    x[1] * track_scale + offset_y]
                                   for x in track_coordinates]

                    self.draw_facility(svg_document, coordinates, facility, track)

        return svg_document.tostring()

    def draw_tracks(self, document: str, coordinates: List[List[int]], track: Track):
        """
        Draw the tracks

        :param document: svg document
        :param coordinates: list of coordinates
        :param track: track object
        """

        if track.type == RAILROAD:
            self.draw_railroad(document, coordinates, track.id)
        elif track.type == BUMPER:
            self.draw_bumper(document, coordinates, track.id)

    def draw_bumper(self, document: str, coordinates: List[List[int]], id: int):
        """
        Draw a bumper

        :param document: svg document
        :param coordinates: list of coordinates
        :param id: the id of the bumper
        """

        x = coordinates[0][0]
        y = coordinates[0][1]
        x2 = coordinates[1][0]
        y2 = coordinates[1][1]

        points = "M" + str(x) + ' ' + str((y + 10)) + ' ' + str(x) + ' ' + str((y - 10))\
                 + "M" + str(x) + ' ' + str(y) + ' ' + str(x2) + ' ' + str(y2)

        line = document.path(d=points, stroke="#121212", stroke_width=6,
                             id="track-" + str(id), fill="none")

        document.add(line)

    def draw_railroad(self, document: str, coordinates: List[List[int]], id: int):
        """
        Draw a railroad

        :param document: svg document
        :param coordinates: list of coordinates
        :param id: the id of the railroad
        """

        points = "M" + ' '.join(str(r) for v in coordinates for r in v)
        line = document.path(d=points, stroke="#121212",
                             stroke_width=6, id="track-" + str(id), fill="none")

        document.add(line)

    def draw_facility(self, document: str, coordinates, facility: List[List[int]], track: Facility):
        """
        Draw a facility

        :param document: svg document
        :param coordinates: list of coordinates
        :param facility: the facility
        """

        config = current_app.vis_config["facilities"][str(facility.id)]

        points = "M" + ' '.join(str(r) for v in coordinates for r in v)
        line = document.path(d=points, stroke=config["color"],
                             stroke_width=18,
                             id="facility-" + str(facility.id) + "-" + str(track),
                             fill="none", stroke_linecap="round")
        document.add(line)

        text = document.text("", font_size=12, fill='#fff')

        document.add(text)
        textpath = document.textPath(path=line, text=facility.type, startOffset='50%',
                                     method='align', spacing='auto', text_anchor='middle',
                                     alignment_baseline='central')

        text.add(textpath)
