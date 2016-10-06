var apiUrl = 'http://star.lesterpig.com/'

module.exports = {

  getStops: function(cb) {
    var req = new XMLHttpRequest()
    req.open('GET', apiUrl + 'stops/')
    req.onload = function() {
      if (req.readyState === 4 && req.status === 200) {
        cb(null, JSON.parse(req.responseText))
      } else {
        console.log(req)
        cb('Internal API error', {})
      }
    }
    req.send(null)
  },

  getTimes : function(stops, cb) {
    var req = new XMLHttpRequest()
    req.open('GET', apiUrl + 'next-buses/' + stops.join(','))
    req.onload = function () {
      if (req.readyState === 4) {
        if (req.status === 200) {
          var times;
          try {
            times = JSON.parse(req.responseText)
          } catch(e) {
            return cb('Malformed JSON')
          }
          cb(null, times.slice(0,10))
        } else {
          cb('Internal API error')
        }
      }
    }
    req.send(null)
  }

}
