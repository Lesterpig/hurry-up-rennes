process.stdin.setEncoding('utf8')

var buffer = ""

process.stdin.on('readable', function() {
  var chunk = process.stdin.read()
  if(!chunk) {
    return
  }

  buffer += chunk
    .replace(/\"/g,      '')
    .replace(/[àâä]/g,  'a')
    .replace(/[éèêë]/g, 'e')
    .replace(/[îï]/g,   'i')
    .replace(/[ôö]/g,   'o')
    .replace(/[ùûü]/g,  'u')

})

process.stdin.on('end', function() {

  var out = {}

  buffer = buffer.split('\n')
  buffer.forEach(function(line, i) {

    if(i === 0) {
      return
    }

    var cells = line.split(',')

    if(cells.length < 6 || !cells[2]) {
      return
    }

    if(!out[cells[2]]) {
      out[cells[2]] = []
    }

    out[cells[2]].push({
      id  : Number(cells[0]),
      lat : Number(cells[4]),
      lon : Number(cells[5])
    })

  })

  process.stdout.write(JSON.stringify(out))

})
