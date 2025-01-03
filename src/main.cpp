#include "ui.h"
#include "engine/engine.h"

int main(int argc, char** argv) {
	Engine engine;

	engine.loadROM("../../../programs/IBM Logo.ch8");

	UI ui(&engine);
	ui.run();
	
	return 0;
}