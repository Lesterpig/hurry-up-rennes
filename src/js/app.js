
/* Start file for embedded javascript */

var stops         = require('../../data/stops.json')
var math          = require('./math.js')
var communication = require('./communication.js')
var api           = require('./api.js')

var savedStops = []

Pebble.addEventListener('ready', function() {

  if(localStorage.bookmarks) {
    try {
      savedStops = JSON.parse(localStorage.bookmarks)
      communication.sendStops(savedStops, false)
    } catch(e) {}
  }

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

    var asked_times_index = Number(e.payload.KEY_ASK_STOP);
    var asked_book_index  = Number(e.payload.KEY_ASK_BOOK);

    if(!isNaN(asked_times_index) && savedStops[asked_times_index]) {

      // Request API
      api.getTimes(savedStops[asked_times_index].ids, function(err, res) {
        if(err) {
          return console.log('Error while fetching API: ' + err)
        }
        communication.sendTimes(res)
      })

    }

    if(!isNaN(asked_book_index)) {
      manageBookmark(asked_book_index)
    }
  }
);

function locationSuccess(pos) {

  console.log(JSON.stringify(pos))

  var lat = pos.coords.latitude
  var lon = pos.coords.longitude

  var bests = [];

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

    if (stopData.dist <= 1) {
      bests.push(stopData)
    }
  }

  bests.sort(function(a,b) { return a.dist - b.dist }) // ascending order
  bests.splice(10) // get only the 10 first stops

  bests.forEach(function(e,i) {
    savedStops[10 + i] = e
  })

  communication.sendStops(bests, true)

}

function manageBookmark(index) {

  if (index < 10) { // Remove bookmark
    savedStops[index] = undefined;
  }

  var bookmarks = []
  var nb = 0
  for (var i = 0; i < 10; i++) {
    if (savedStops[i]) {
      bookmarks.push(savedStops[i])
      nb++
    }
  }

  if (index >= 10) {
    var msg
    if (nb === 10) {
      msg = 'You have reached the max number of bookmarks :('
    }
    else {
      bookmarks.push(savedStops[index])
      msg = 'Bookmark ' + savedStops[index].name.toUpperCase() + ' added!'
    }

    // Emit notification to watch
    Pebble.showSimpleNotificationOnPebble('Hurry Up!', msg)
  }

  // Save in phone

  localStorage.bookmarks = bookmarks.length > 0 ? JSON.stringify(bookmarks) : undefined

  // Emit to watch and save in RAM

  bookmarks.forEach(function(e,i) {
    savedStops[i] = e
  })

  communication.sendStops(bookmarks, false)
}
