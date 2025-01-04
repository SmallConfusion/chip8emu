#pragma once
#include <bitset>
#include <chrono>
#include <cstdint>
#include <stack>
#include <vector>
#include "engine/engineTypes.h"

class UI;

class Engine {
	friend class Debug;

   public:
	Engine();

	void loadROM(const char* filename);
	void loadROM(const byte* bytes, int length);
	void update(const UI& ui);

	const std::bitset<64 * 32>& getDisplay() const;

   protected:
	std::bitset<64 * 32> display;

	std::vector<byte> ram;
	std::vector<byte> vreg;

	addr pc, ireg;
	byte timer, sound;

	std::stack<addr> stack;

   private:
	int waitUntilKeyReleased = -1;

	double nextTimerDec = 0;
	double nextInstruction = 0;

	double cps = 650;

	bool stepMode = false;
	bool shiftCompat = false;  // https://chip8.gulrak.net/#quirk5

	// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#bnnn-jump-with-offset
	bool bxnnCompat = false;

	// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx1e-add-to-index
	bool addIndexOverflowCompat = false;

	// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx55-and-fx65-store-and-load-memory
	bool memoryIncI = false;

	// https://chip8.gulrak.net/#quirk4at
	bool binaryResetVFCompat = true;

	void reset();
	void loadSystem();
	bool cycle(const bool keymap[16]);
	void draw(byte xr, byte yr, byte height);
};
