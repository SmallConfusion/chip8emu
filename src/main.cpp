#include <filesystem>
#include "engine/engine.h"
#include "engine/engineTypes.h"
#include "ui.h"

int main(int argc, char** argv) {
	Engine engine;

	bool loaded = false;
	for (auto path : {"chip8-test-suite/bin/1-chip8-logo.ch8",
					  "../../../chip8-test-suite/bin/1-chip8-logo.ch8"}) {
		if (std::filesystem::exists(path)) {
			engine.loadROM(path);
			loaded = true;
			break;
		}
	}

	if (!loaded) {
		const byte r[] = {0x12, 0x00};
		engine.loadROM(r, 2);
	}

	UI ui(&engine);
	ui.run();

	return 0;
}