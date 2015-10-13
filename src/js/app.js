
/* Start file for embedded javascript */

var stops         = require('../../data/stops.json')
var math          = require('./math.js')
var communication = require('./communication.js')
var api           = require('./api.js')

var savedStops = []

Pebble.addEventListener('ready', function() {
  console.log('Ready! Retrieving location...')
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    function(err) { console.error("Unable to retrieve location!") },
    {timeout: 15000, maximumAge: 60000}
  );
});

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + JSON.stringify(e.payload));
    var stopIndex = Number(e.payload['KEY_ASK_STOP']);

    if(!savedStops[stopIndex]) {
      return
    }

    // Request API
    api.getTimes(savedStops[stopIndex].ids, function(err, res) {
      if(err) {
        return console.log('Error while fetching API: ' + err)
      }
      communication.sendTimes(res)
    })

  }
);

function locationSuccess(pos) {

  console.log(JSON.stringify(pos))

  var lat = pos.coords.latitude
  var lon = pos.coords.longitude

  var bests = [];
  function refreshOrder() {
    bests.sort(function(a,b) { return a.dist - b.dist }) // ascending order
  }

  for (var stopName in stops) {
    var stop = stops[stopName]
    var stopData = { ids: [], dist: Infinity, name: stopName }
    for (var i = 0; i < stop.length; i++) {
      var thisDistance = math.getDistanceFromLatLonInKm(lat, lon, stop[i].lat, stop[i].lon)
      stopData.ids.push(stop[i].id)
      if (stopData.dist > thisDistance) {
        stopData.dist = thisDistance
      }
    }

    // Update bests if needed

    if (bests.length < 3) { // Available seat, take it
      bests.push(stopData)
      refreshOrder()
    } else if(bests[bests.length - 1].dist > stopData.dist) { // Replace the farther stop
      bests[bests.length - 1] = stopData
      refreshOrder()
    }
  }

  savedStops = bests
  communication.sendStops(savedStops)

}
