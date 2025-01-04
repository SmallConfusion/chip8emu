#include "engine.h"
#include "engine/rand.h"
#include <fstream>

constexpr int RAMSIZE = 1 << 12;

Engine::Engine() {
	ram.resize(RAMSIZE, 0);
	vreg.resize(16, 0);

	reset();
}

void Engine::loadROM(const char* filename) {
	reset();

	std::ifstream file(filename);

	for (addr i = 512; i < RAMSIZE; i++) {
		int g = file.get();

		if (g == EOF) {
			break;
		}

		ram[i] = g;
	}
}

void Engine::update() {
	for (int i = 0; i < 64 * 32; i++) {
		if (rand::get() % 2 == 0) {
			display.flip(i);
		}
	}
}

const std::bitset<64 * 32>& Engine::getDisplay() const {
	return display;
}

void Engine::reset() {
	for (byte& a : ram) {
		a = 0;
	}

	for (byte& v : vreg) {
		v = 0;
	}

	while (!stack.empty()) {
		stack.pop();
	}

	display.reset();

	pc = 0;
	ireg = 0;
	timer = 0;
	sound = 0;

	loadSystem();
}

void Engine::loadSystem() {
	constexpr byte FONT[] = {
		// clang-format off
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		// clang-format on
	};

	constexpr int FONT_LENGTH = sizeof(FONT) / sizeof(byte);
	constexpr addr FONT_ADDR = 0x050;

	for (int i = 0; i < FONT_LENGTH; i++) {
		ram[FONT_ADDR + i] = FONT[i];
	}

	pc = 512;
}
