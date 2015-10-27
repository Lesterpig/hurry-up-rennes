module.exports = {

  sendStops : function(stops, nearest) {

    var key = 'KEY_STOPS_BOOK'
    if(nearest) {
      key = 'KEY_STOPS_NEAR'
    }

    var data = []
    stops.forEach(function(s) {
      data.push(s.name.substr(0,20) + '\0') // data should not be longer than 20*10+10 < 256B
    })

    var obj  = {}
    obj[key] = data

    console.log('Sending ' + JSON.stringify(obj))

    Pebble.sendAppMessage(
      obj,
      function() {},
      function(e) { console.error('Error: ' + e) }
    )

  },

  sendTimes : function(times) {

    console.log('Sending stop times')

    var data = []
    var msg

    if(times.length === 0) {

      msg = {
        'KEY_NO_STOP_TIMES' : 1
      }

    } else {

      times.forEach(function(t) {

        data.push(t.accurate ? 1 : 0) // 1 byte
        data.push(strToData(t.line, 4))
        data.push(strToData(t.destination, 21))
        data.push(strToData(t.time + '\'', 4))

      })

      msg = {
        'KEY_STOP_TIMES' : data.slice(0,50)
      }

    }

    Pebble.sendAppMessage(msg, function() {}, function() {});

  }

}

function strToData(value, qty) {
  var out = value.substr(0, qty - 1) + '\0'
  var missing = qty - out.length
  for(var i = 0; i < missing; i++) {
    out += 'x'
  }
  return out
}
