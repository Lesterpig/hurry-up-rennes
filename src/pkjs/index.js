/* Start file for embedded javascript */
console.log('Loaded javascript for Hurry Up!')

var math          = require('./math.js')
var communication = require('./communication.js')
var api           = require('./api.js')

var stops = {}
var savedStops = []

Pebble.addEventListener('ready', function() {
  if(localStorage.bookmarks) {
    try {
      savedStops = JSON.parse(localStorage.bookmarks)
    } catch(e) {}
  }

  console.log('Retrieving stop list...')
  api.getStops(function(err, fetchedStops) {
    stops = fetchedStops
    console.log('Ready! Retrieving location...')
    navigator.geolocation.getCurrentPosition(
      locationSuccess,
      function(err) { console.error("Unable to retrieve location!") },
      {timeout: 30000, maximumAge: 60000}
    )
  })
})

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + JSON.stringify(e.payload));

    var asked_times_index = Number(e.payload.ASK_STOP);
    var asked_book_index  = Number(e.payload.ASK_BOOK);
    var asked_timeo_index = Number(e.payload.ASK_TIMEO);

    if (!isNaN(asked_times_index) && savedStops[asked_times_index]) {
      // Request API
      api.getTimes(savedStops[asked_times_index].ids, function(err, res) {
        if(err) {
          return console.log('Error while fetching API: ' + err)
        }
        communication.sendTimes(res)
      })
    }

    if (!isNaN(asked_book_index)) {
      manageBookmark(asked_book_index)
    }

    if (!isNaN(asked_timeo_index)) {
      // Find stop by its id
      var timeoStop = false;
      for (var name in stops) {
        stops[name].forEach(function(e) {
          if (e[0] === asked_timeo_index) {
            timeoStop = true;
          }
        })
        if (timeoStop) {
          savedStops[20] = { ids: [asked_timeo_index], name: name };
          manageBookmark(20);
          break;
        }
      }
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
      var thisDistance = math.getDistanceFromLatLonInKm(lat, lon, stop[i][1], stop[i][2])
      stopData.ids.push(stop[i][0])
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
