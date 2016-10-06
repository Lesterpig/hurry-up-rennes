default:
	pebble build

start:
	pebble install --logs --emulator basalt

real:
	pebble install --logs --cloudpebble
