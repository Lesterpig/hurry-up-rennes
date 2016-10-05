default:
	rm -rf build
	pebble build

start:
	pebble install --logs --emulator aplite

real:
	pebble install --logs --cloudpebble
