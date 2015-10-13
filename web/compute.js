var moment = require('moment')

module.exports = {

  nextBuses: function(data, output, cb) {

    var current_time = moment(data['@attributes'].localdatetime)

    if(!data.stopline) {
      return cb(null)
    }

    if(!data.stopline.length) {
      data.stopline = [data.stopline]
    }

    data.stopline.forEach(function(line) {

      if(!line.departures.departure.length) {
        // Single element
        line.departures.departure = [line.departures.departure]
      }

      line.departures.departure.forEach(function(depart) {
        var time = Math.floor((moment(depart.content) - current_time)/(1000 * 60))
        output.push({
          line        : cleanLine(line.route),
          accurate    : depart['@attributes'].accurate === '1',
          destination : cleanStr(depart['@attributes'].headsign),
          time        : time < 0 ? 0 : time
        })
      })

    })

    cb(null)

  }

}

function cleanLine(line) {

  line = line.replace(/^0*/g, '')

  if(line === '2' || line === '3' || line === '4') {
    line = 'C' + line
  }
  else if(line === '121') {
    line = 'N1'
  }
  else if(line === '122') {
    line = 'N2'
  }
  else if(line === '158') {
    line = 'KL'
  }

  return line

}

function cleanStr(str) {

  return str
    .replace(/\s\|?\s+/g,' ')
    .replace(/[àâä]/g,  'a')
    .replace(/[éèêë]/g, 'e')
    .replace(/[îï]/g,   'i')
    .replace(/[ôö]/g,   'o')
    .replace(/[ùûü]/g,  'u')

}
