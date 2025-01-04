#include "engine.h"
#include <imgui.h>
#include <cmath>
#include <fstream>
#include <print>
#include "engine/engineTypes.h"
#include "engine/instructions.h"
#include "engine/rand.h"
#include "engine/sound.h"
#include "ui.h"
#include "util.h"

constexpr int RAMSIZE = 1 << 12;
constexpr addr FONT_ADDR = 0x050;

Engine::Engine() {
	nextInstruction = nextTimerDec = SDL_GetTicks();

	ram.resize(RAMSIZE, 0);
	vreg.resize(16, 0);

	reset();
}

void Engine::loadAudio() {
	soundPlayer = std::make_unique<Sound>();
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

void Engine::loadROM(const byte* bytes, int length) {
	for (int i = 0; i < length; i++) {
		ram[512 + i] = bytes[i];
	}
}

void Engine::update(const UI& ui) {
	ui.debugInfo([&] {
		ImGui::Begin("Engine config");

		ImGui::Checkbox("Step mode", &stepMode);
		ImGui::InputDouble("Cycles per second", &cps);

		ImGui::Checkbox("Bit shift comatability (8XY6 and 8XYE)", &shiftCompat);
		ImGui::Checkbox("Jump compatability (bxnn (on) vs bnnn (off))",
						&bxnnCompat);
		ImGui::Checkbox("Add to index overflow compatability",
						&addIndexOverflowCompat);
		ImGui::Checkbox("Increment index when saving and loading ram",
						&memoryIncI);
		ImGui::Checkbox("Binary operations reset VF compatability",
						&binaryResetVFCompat);

		ImGui::End();

		soundPlayer->showUI();
	});

	unsigned int time = SDL_GetTicks();

	while (time > nextTimerDec) {
		nextTimerDec += 1000.0 / 60.0;

		if (timer > 0) {
			timer--;
		}

		if (sound > 0) {
			sound--;
		}
	}

	soundPlayer->playing = sound > 0;

	if (stepMode) {
		if (ui.step) {
			cycle(ui.keymap);
		}
	} else {
		while (nextInstruction < time) {
			cycle(ui.keymap);
			nextInstruction += 1000 / cps;
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

	for (int i = 0; i < FONT_LENGTH; i++) {
		ram[FONT_ADDR + i] = FONT[i];
	}

	pc = 512;
}

bool Engine::cycle(const bool keymap[16]) {
	inst fetched = (ram[pc] << 8) + ram[pc + 1];
	pc += 2;

	inst nibble = fetched & 0xF000;

	auto noInst = [&]() {
		std::println("Instruction {:} at {:} not recognized!",
					 util::instructionToHex(fetched), util::addrToHex(pc - 2));
	};

	if (fetched == I_CLEAR_SCREEN) {
		display.reset();

	} else if (nibble == I_JUMP) {
		addr jumpLoc = fetched & I_JUMP_MASK;
		pc = jumpLoc;

	} else if (nibble == I_SET_REG) {
		byte reg = (fetched & I_SET_REG_REG_MASK) >> I_SET_REG_REG_SHIFT;
		byte val = fetched & I_SET_REG_VALUE_MASK;
		vreg[reg] = val;

	} else if (nibble == I_ADD_REG) {
		byte reg = (fetched & I_ADD_REG_REG_MASK) >> I_ADD_REG_REG_SHIFT;
		byte val = fetched & I_ADD_REG_VALUE_MASK;
		vreg[reg] += val;

	} else if (nibble == I_SET_INDEX) {
		addr val = fetched & I_SET_INDEX_MASK;
		ireg = val;

	} else if (nibble == I_DRAW) {
		byte xr = (fetched & I_DRAW_X_MASK) >> I_DRAW_X_SHIFT;
		byte yr = (fetched & I_DRAW_Y_MASK) >> I_DRAW_Y_SHIFT;
		byte height = fetched & I_DRAW_HEIGHT_MASK;

		draw(xr, yr, height);

		// Conditional skips 3XNN 4XNN 5XY0 9XY0
	} else if (nibble == 0x3000) {
		byte r = (fetched & 0x0F00) >> 8;
		byte x = fetched & 0x00FF;

		if (vreg[r] == x) {
			pc += 2;
		}

	} else if (nibble == 0x4000) {
		byte r = (fetched & 0x0F00) >> 8;
		byte x = fetched & 0x00FF;

		if (vreg[r] != x) {
			pc += 2;
		}

	} else if (nibble == 0x5000) {
		byte rx = (fetched & 0x0F00) >> 8;
		byte ry = (fetched & 0x00F0) >> 4;

		if (vreg[rx] == vreg[ry]) {
			pc += 2;
		}

	} else if (nibble == 0x9000) {
		byte rx = (fetched & 0x0F00) >> 8;
		byte ry = (fetched & 0x00F0) >> 4;

		if (vreg[rx] != vreg[ry]) {
			pc += 2;
		}

		// Subroutines
	} else if (nibble == 0x2000) {
		addr subroutine = fetched & 0x0FFF;

		stack.push(pc);
		pc = subroutine;

	} else if (fetched == I_RETURN) {
		pc = stack.top();
		stack.pop();

		// Logic and arithmatic instructions 8XY0
	} else if (nibble == 0x8000) {
		int x = (fetched & 0x0F00) >> 8;
		int y = (fetched & 0x00F0) >> 4;

		int end = fetched & 0x000F;

		switch (end) {
			case 0:
				vreg[x] = vreg[y];

				if (binaryResetVFCompat) {
					vreg[0xF] = 0;
				}

				break;

			case 1:
				vreg[x] |= vreg[y];

				if (binaryResetVFCompat) {
					vreg[0xF] = 0;
				}

				break;

			case 2:
				vreg[x] &= vreg[y];

				if (binaryResetVFCompat) {
					vreg[0xF] = 0;
				}

				break;

			case 3:
				vreg[x] ^= vreg[y];

				if (binaryResetVFCompat) {
					vreg[0xF] = 0;
				}

				break;

			case 4: {
				short res = vreg[x] + vreg[y];
				vreg[x] = res & 0xFF;
				vreg[0xF] = res > 0xFF;
			} break;

			case 5: {  // Subtract - OUT OF ORDER
				byte carry = vreg[x] >= vreg[y];
				vreg[x] = vreg[x] - vreg[y];
				vreg[0xF] = carry;
			} break;

			case 7: {  // Subtract - OUT OF ORDER
				byte carry = vreg[y] >= vreg[x];
				vreg[x] = vreg[y] - vreg[x];
				vreg[0xF] = carry;
			} break;

			case 6: {  // Shift - OUT OF ORDER
				if (shiftCompat) {
					vreg[x] = vreg[y];
				}

				byte carry = vreg[x] & 0b1;
				vreg[x] = vreg[x] >> 1;
				vreg[0xF] = carry;
			} break;

			case 0xE: {	 // Shift - OUT OF ORDER
				if (shiftCompat) {
					vreg[x] = vreg[y];
				}

				byte carry = (vreg[x] & 0b10000000) > 0;
				vreg[x] = vreg[x] << 1;
				vreg[0xF] = carry;
			} break;

			default:
				noInst();
		}

		// Jump with offset
	} else if (nibble == 0xB000) {
		addr to;

		if (bxnnCompat) {
			addr add = fetched & 0x0FFF;
			byte r = (fetched & 0x0F00) >> 8;
			to = vreg[r] + add;
		} else {
			addr add = fetched & 0x0FFF;
			to = vreg[0] + add;
		}

		pc = to;

		// Random
	} else if (nibble == 0xC000) {
		byte reg = (fetched & 0x0F00) >> 8;
		byte andVal = fetched & 0x00FF;

		byte res = andVal & rand::get(0, 0xFF);

		vreg[reg] = res;

		// Skip if key
	} else if (nibble == 0xE000) {
		byte r = (fetched & 0x0F00) >> 8;

		bool pressed;
		if ((fetched & 0x00FF) == 0x009E) {
			pressed = true;
		} else if ((fetched & 0x00FF) == 0x00A1) {
			pressed = false;
		} else {
			noInst();
			return false;
		}

		bool skip = pressed == keymap[vreg[r]];

		if (skip) {
			pc += 2;
		}

		// Timers
	} else if ((fetched & 0xF0FF) == 0xF007) {
		byte r = (fetched & 0x0F00) >> 8;
		vreg[r] = timer;

	} else if ((fetched & 0xF0FF) == 0xF015) {
		byte r = (fetched & 0x0F00) >> 8;
		timer = vreg[r];

	} else if ((fetched & 0xF0FF) == 0xF018) {
		byte r = (fetched & 0x0F00) >> 8;
		sound = vreg[r];

		// Add to index
	} else if ((fetched & 0xF0FF) == 0xF01E) {
		byte r = (fetched & 0x0F00) >> 8;
		ireg += vreg[r];

		if (!addIndexOverflowCompat) {
			vreg[0xF] = ireg > 0x0FFF;
		}

		// Get key
	} else if ((fetched & 0xF0FF) == 0xF00A) {
		byte r = (fetched & 0x0F00) >> 8;

		if (waitUntilKeyReleased > 0) {
			if (!keymap[waitUntilKeyReleased]) {
				vreg[r] = waitUntilKeyReleased;
				waitUntilKeyReleased = -1;
				return false;
			}

			pc -= 2;
			return true;
		}

		for (int i = 0; i < 16; i++) {
			if (keymap[i]) {
				waitUntilKeyReleased = i;
			}
		}

		pc -= 2;
		return true;

		// Font character
	} else if ((fetched & 0xF0FF) == 0xF029) {
		byte r = (fetched & 0x0F00) >> 8;
		ireg = FONT_ADDR + 5 * vreg[r];

		// Binary decimal conversion
	} else if ((fetched & 0xF0FF) == 0xF033) {
		byte r = (fetched & 0x0F00) >> 8;
		byte n = vreg[r];

		byte c1 = n / 100;
		byte c2 = (n % 100) / 10;
		byte c3 = n % 10;

		ram[ireg] = c1;
		ram[ireg + 1] = c2;
		ram[ireg + 2] = c3;

		// Memory
	} else if ((fetched & 0xF0FF) == 0xF055) {
		byte er = (fetched & 0x0F00) >> 8;

		for (int i = 0; i <= er; i++) {
			ram[ireg + i] = vreg[i];
		}

		if (memoryIncI) {
			ireg += er + 1;
		}

	} else if ((fetched & 0xF0FF) == 0xF065) {
		byte er = (fetched & 0x0F00) >> 8;

		for (int i = 0; i <= er; i++) {
			vreg[i] = ram[ireg + i];
		}

		if (memoryIncI) {
			ireg += er + 1;
		}

	} else {
		noInst();
	}

	return false;
}

void Engine::draw(byte xr, byte yr, byte height) {
	constexpr int SCREEN_WIDTH = 64;
	constexpr int SCREEN_HEIGHT = 32;
	constexpr int SPRITE_WIDTH = 8;

	int xb = vreg[xr] & 63;
	int yb = vreg[yr] & 31;

	vreg[0xF] = 0;

	for (int row = 0; row < height && row + yb < SCREEN_HEIGHT; row++) {
		byte spriteRow = ram[ireg + row];

		for (int i = 0; i < SPRITE_WIDTH; i++) {
			if ((spriteRow & (0b10000000 >> i)) == 0) {
				continue;
			}

			int xp = xb + i;
			if (xp >= SCREEN_WIDTH) {
				continue;
			}

			int yp = yb + row;

			int displayIndex = xp + yp * SCREEN_WIDTH;

			if (display[displayIndex]) {
				vreg[0xF] = 1;
			}

			display.flip(displayIndex);
		}
	}
}
