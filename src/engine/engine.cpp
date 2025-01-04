#include "engine.h"
#include <fstream>
#include <print>
#include "engine/engineTypes.h"
#include "engine/instructions.h"
#include "engine/rand.h"
#include "ui.h"
#include "util.h"

constexpr int RAMSIZE = 1 << 12;

Engine::Engine() {
	ram.resize(RAMSIZE, 0);
	vreg.resize(16, 0);

	reset();
}

void Engine::loadROM(const char* filename) {
	reset();

	std::ifstream file(filename, std::ios::in | std::ios::binary);

	for (addr i = 512; i < RAMSIZE; i++) {
		int g = file.get();

		if (g == EOF) {
			if (i == 512) {
				std::println("Error, rom file not found or is empty.");
			}

			break;
		}

		ram[i] = g;
	}
}

void Engine::update(const UI& ui) {
	constexpr bool STEP_MODE = false;

	int cycles = 700 / 165;

	if (STEP_MODE) {
		cycles = ui.step ? 1 : 0;
	}

	for (; cycles > 0; cycles--) {
		cycle();
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

void Engine::cycle() {
	inst fetched = (ram[pc] << 8) + ram[pc + 1];
	pc += 2;

	if (fetched == I_CLEAR_SCREEN) {
		display.reset();

	} else if ((fetched & I_JUMP_CHECK) == I_JUMP) {
		addr jumpLoc = fetched & I_JUMP_MASK;
		pc = jumpLoc;

	} else if ((fetched & I_SET_REG_CHECK) == I_SET_REG) {
		byte reg = (fetched & I_SET_REG_REG_MASK) >> I_SET_REG_REG_SHIFT;
		byte val = fetched & I_SET_REG_VALUE_MASK;
		vreg[reg] = val;

	} else if ((fetched & I_ADD_REG_CHECK) == I_ADD_REG) {
		byte reg = (fetched & I_ADD_REG_REG_MASK) >> I_ADD_REG_REG_SHIFT;
		byte val = fetched & I_ADD_REG_VALUE_MASK;
		vreg[reg] += val;

	} else if ((fetched & I_SET_INDEX_CHECK) == I_SET_INDEX) {
		addr val = fetched & I_SET_INDEX_MASK;
		ireg = val;

	} else if ((fetched & I_DRAW_CHECK) == I_DRAW) {
		byte xr = (fetched & I_DRAW_X_MASK) >> I_DRAW_X_SHIFT;
		byte yr = (fetched & I_DRAW_Y_MASK) >> I_DRAW_Y_SHIFT;
		byte height = fetched & I_DRAW_HEIGHT_MASK;

		draw(xr, yr, height);

		// Conditional skips 3XNN 4XNN 5XY0 6XY0
	} else if ((fetched & 0xF000) == 0x3000) {
		byte r = (fetched & 0x0F00) >> 8;
		byte x = fetched & 0x00FF;

		if (vreg[r] == x) {
			pc += 2;
		}

	} else if ((fetched & 0xF000) == 0x4000) {
		byte r = (fetched & 0x0F00) >> 8;
		byte x = fetched & 0x00FF;

		if (vreg[r] != x) {
			pc += 2;
		}

	} else if ((fetched & 0xF000) == 0x5000) {
		byte rx = (fetched & 0x0F00) >> 8;
		byte ry = (fetched & 0x00F0) >> 4;

		if (vreg[rx] == vreg[ry]) {
			pc += 2;
		}

	} else if ((fetched & 0xF000) == 0x5000) {
		byte rx = (fetched & 0x0F00) >> 8;
		byte ry = (fetched & 0x00F0) >> 4;

		if (vreg[rx] != vreg[ry]) {
			pc += 2;
		}

		// Subroutines
	} else if ((fetched & 0xF000) == 0x2000) {
		addr subroutine = fetched & 0x0FFF;

		stack.push(pc);
		pc = subroutine;

	} else if (fetched == I_RETURN) {
		pc = stack.top();
		stack.pop();

	} else {
		std::println("Instruction {:} at {:} not recognized!",
					 util::instructionToHex(fetched), util::addrToHex(pc - 2));
	}
}

void Engine::draw(byte xr, byte yr, byte height) {
	constexpr int SCREEN_WIDTH = 64;
	constexpr int SCREEN_HEIGHT = 32;
	constexpr int SPRITE_WIDTH = 8;

	int xb = vreg[xr] % SCREEN_WIDTH;
	int yb = vreg[yr] % SCREEN_HEIGHT;

	vreg[0xF] = 0;

	for (int row = 0; row < height && row + yb < SCREEN_HEIGHT; row++) {
		byte spriteRow = ram[ireg + row];

		for (int i = 0; i < SPRITE_WIDTH; i++) {
			if ((spriteRow & (0b1 << (SPRITE_WIDTH - 1 - i))) == 0) {
				continue;
			}

			int xp = xb + i;
			if (xp >= SCREEN_WIDTH) {
				continue;
			}

			int yp = yb + row;

			int displayIndex = xp + yp * SCREEN_WIDTH;

			if (display.test(displayIndex)) {
				vreg[0xF] = 1;
			}

			display.flip(displayIndex);
		}
	}
}
