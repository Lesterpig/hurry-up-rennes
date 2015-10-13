module.exports = {

  sendStops : function(stops) {

    Pebble.sendAppMessage(
      {
        'KEY_STOPS' : stops[0].name + '\0' + stops[1].name + '\0' + stops[2].name + '\0'
      },
      function() {},
      function(e) { console.error('Error: ' + e) }
    )

  },

  sendTimes : function(times) {

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
