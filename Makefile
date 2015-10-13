default:
	rm -rf build
	browserify src/js/app.js -o src/js/pebble-js-app.js
	pebble build

start:
	pebble install --logs --emulator aplite

real:
	pebble install --logs --cloudpebble
