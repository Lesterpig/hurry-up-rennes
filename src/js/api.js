module.exports = {

  getTimes : function(stops, cb) {

    var req = new XMLHttpRequest()
    req.open('GET', 'http://star.lesterpig.com/next-buses/' + stops.join(','))
    req.onload = function () {
      if (req.readyState === 4) {
        if (req.status === 200) {
          var times;
          try {
            times = JSON.parse(req.responseText)
          } catch(e) {
            return cb('Malformed JSON')
          }
          cb(null, times)
        } else {
          cb('Internal API error')
        }
      }
    }

    req.send(null);

  }


}
