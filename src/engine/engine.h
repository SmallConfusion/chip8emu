#pragma once
#include <SDL.h>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <memory>
#include <stack>
#include <vector>
#include "engine/engineTypes.h"
#include "engine/memory.h"
#include "engine/sound.h"

class UI;

class Engine {
	friend class Debug;

   public:
	Engine();

	void loadAudio();

	void loadROM(const char* filename);
	void loadROM(const byte* bytes, int length);
	void update(const UI& ui);

	const std::bitset<64 * 32>& getDisplay() const;

   protected:
	std::bitset<64 * 32> display;

	Memory ram;
	std::vector<byte> vreg;

	addr pc, ireg;
	byte timer, sound;

	std::stack<addr> stack;

   private:
	int waitUntilKeyReleased = -1;

	double nextTimerDec = 0;
	double nextInstruction = 0;

	double cps = 1000;

	bool stepMode = false;
	bool shiftCompat = true;  // https://chip8.gulrak.net/#quirk5

	// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#bnnn-jump-with-offset
	bool bxnnCompat = false;

	// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx1e-add-to-index
	bool addIndexOverflowCompat = true;

	// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx55-and-fx65-store-and-load-memory
	bool memoryIncI = true;

	// https://chip8.gulrak.net/#quirk4at
	bool binaryResetVFCompat = true;

	bool spriteWrap = false;

	void reset();

	std::unique_ptr<Sound> soundPlayer;

	void loadSystem();
	bool cycle(const bool keymap[16]);
	void draw(byte xr, byte yr, byte height);
};
