var async   = require('async')
   ,cached  = require('request-easy-cache').CachedRequest
   ,app     = require('express')()
   ,compute = require('./compute')
   ,conf    = require('./config.json')

var request = new cached({
  cache: {
    max: 500,
    maxAge: 30000
  }
})

var base_url = 'http://data.keolis-rennes.com/json/?cmd=getbusnextdepartures&version=2.2&key=' + conf.starKey + '&param[mode]=stop'

app.get('/next-buses/:stops', function(req, res) {

  var output = []
  var stops  = req.params.stops.split(',').map(function(e) { return Number(e) })
  var fns    = [] // functions to execute to fetch results

  while(stops.length > 0) {
    (function(stops_set) {
      fns.push(function(cb) {

        var url = base_url

        stops_set.forEach(function(e) {
          url += '&param[stop][]=' + e
        })

        request.get(url, {
          'json' : true
        }, function(err, result, body) {

          if(err || result.statusCode < 200 || result.statusCode >= 400) {
            return cb(err)
          }

          if(!body.opendata
              || !body.opendata.answer
              || !body.opendata.answer.status
              || !body.opendata.answer.status['@attributes']
              || body.opendata.answer.status['@attributes'].code !== "0") {
            return cb(body.opendata.answer.status['@attributes'].message)
          }

          compute.nextBuses(body.opendata.answer.data, output, cb)

        })

      })
    })(stops.splice(0, 10))
  }

  async.parallel(fns, function(e, r) {
    if(e) {
      console.error(e)
      return res.sendStatus(500)
    }

    output.sort(function(a,b) { return a.time - b.time })
    res.send(output)
  });

})

app.listen(conf.port || 3000, function() {
  console.log('-- Hurry Up! web server is ready on port ' + (conf.port || 3000))
})
