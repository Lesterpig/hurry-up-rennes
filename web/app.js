"use strict"

var cached  = require('request-easy-cache').CachedRequest
var moment  = require('moment')
var app     = require('express')()
var conf    = require('./config.json')
var stops   = require('./stops.json')

var request = new cached({
  cache: {
    max    : 500,
    maxAge : 30000
  }
})

const baseUrl = 'https://data.explore.star.fr/api/records/1.0/search/?apikey=' + conf.starKey + '&dataset=tco-bus-circulation-passages-tr&sort=-depart&rows=10&q=idarret:'

app.get('/stops', (req, res) => {
  res.send(stops)
})

app.get('/next-buses/:stops', (req, res) => {
  let output = []
  let stops  = req.params.stops.split(',').map((e) => { return Number(e) })
  let fns    = [] // functions to execute to fetch results

  let q = "["
  stops.forEach((e, i) => {
    if (i > 0) {
      q += " OR "
    }
    q += e
  })

  request.get(baseUrl + q, {
    json: true
  }, (err, result, body) => {
    if (err || result.statusCode < 200 || result.statusCode >= 400) {
      return res.sendStatus(500)
    }

    let output = []
    body.records.forEach((record) => {
      let serverTime = moment(record.record_timestamp)
      let departureTime = moment(record.fields.depart)
      let remainingTime = Math.floor((departureTime - serverTime) / (60000))
      output.push({
        line        : record.fields.nomcourtligne,
        accurate    : record.fields.precision === "Temps réel",
        destination : cleanStr(record.fields.destination),
        time        : remainingTime,
      })
    })

    res.send(output)
  })
})

var port = conf.port || 3000
app.listen(port, function() {
  console.log('-- Hurry Up! api proxy server is ready on port ' + port)
})

function cleanStr(str) {
  return str
    .replace(/\s\|?\s+/g,' ')
    .replace(/[àâä]/g,  'a')
    .replace(/[éèêë]/g, 'e')
    .replace(/[îï]/g,   'i')
    .replace(/[ôö]/g,   'o')
    .replace(/[ùûü]/g,  'u')
}
