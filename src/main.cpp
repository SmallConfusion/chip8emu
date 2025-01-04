#include "ui.h"
#include "engine/engine.h"

int main(int argc, char** argv) {
	Engine engine;

	engine.loadROM("../../../chip8-test-suite/bin/8-scrolling.ch8");

	UI ui(engine);
	ui.run();
	
	return 0;
}