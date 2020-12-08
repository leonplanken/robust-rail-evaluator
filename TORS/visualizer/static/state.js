var svg_layout;

$(document).ready(function () {
    load_layout(refresh_state);
});

function load_layout(cb) {
    /*
     * Load the layout and cache the image for later use
     */
    Snap.load("/engine/location", svg => {
            svg_layout = svg;
            cb();
        }
    );
}

function refresh_state() {
    /*
     * Load a new state
     */
    $.getJSON("/engine/state", function (data) {
        update_info(data);
        update_layout(data);
    });
}

function update_info(data) {
    /*
     * Update state info
     * :param data: the json object with information about the state
     * :type: string
     */
    $("#current-time").html(data.time);
    $("#next-event").html(data.next_event);

    var shunting_units = "";
    $.each(data.tracks, function(i, trains){
        $.each(trains, function(i, train){
            var color = get_train_color(train.id);
            shunting_units += "<p style='color: " + color + ";'>" + train.unique_id.split("-")[0] + ": [" + train.train_units + "]</p>"
        });
    });
    $("#shunting-units").html(shunting_units);
}

function update_layout(data) {
    /*
     * Update the layout (trains and reserved tracks)
     * :param data: the json object with information about the state
     * :type: string
     */

    // clear
    layout = Snap('#layout');

    var svg_width = svg_layout.node.getAttribute("width");
    var svg_height = svg_layout.node.getAttribute("height");

    layout.attr({
        width: svg_width,
        height: svg_height
    });

    layout.clear();
    layout.append(Snap(svg_layout.node.cloneNode(true)));

    // display trains on tracks
    $.each(data.tracks, function (i, trains) {

        // if there are trains on the track
        if (trains.length) {
            var track_line = document.getElementById("track-" + i);
            var segment = track_line.getTotalLength() / trains.length;
            var end = 0;
            var sample_interval = 5;
        }

        // display trains
        $.each(trains, function (i, train) {
            var start = end;
            var points = "M";

            // collect points on track
            for (var t = start; t <= segment * (i + 1) + sample_interval; t += sample_interval) {
                point = track_line.getPointAtLength(t);
                points += point.x + " " + point.y + " ";
                end = t;
            }

            // draw train
            var train_line = layout.path(d = points);

            var color = get_train_color(train.id);

            train_line.attr({
                id: "train-" + train.unique_id,
                strokeWidth: 14,
                stroke: color,
                fill: "none"
            });

            // display direction
            if (train.direction && !train.in_neutral && !train.moving) {

                var point1 = train_line.getPointAtLength(10);
                var point2 = train_line.getPointAtLength(train_line.getTotalLength() - 10);

                // calculate which end of the train is closest to direction point
                var a = point1.x - train.direction[0];
                var b = point1.y - train.direction[1];
                var c = Math.sqrt(a * a + b * b);

                var a2 = point2.x - train.direction[0];
                var b2 = point2.y - train.direction[1];
                var c2 = Math.sqrt(a2 * a2 + b2 * b2);

                //draw circle
                var center = (c < c2) ? point1 : point2;
                var direction = layout.circle(center.x, center.y, 4);

                direction.attr({
                    id: "direction-" + train.unique_id,
                    fill: "#f0f0f0"
                });
            }
        });
    });

    // display reserved tracks
    $.each(data.reserved_tracks, function (i, track) {
        var reserved_track = $("#track-" + track);
        if (reserved_track.length) {
            reserved_track.attr({
                stroke: "#a12221"
            });
        }
    });
}

function get_train_color(train) {
    // get color for train
    var x = train;
    x = x ^ x << 13;
    x = x ^ x >> 17;
    x = x ^ x << 5;
    x = x % 16777215;
    return '#' + (0x1000000 + x * 0xffffff).toString(16).substr(1, 6);
}