#pragma once
#include <bitset>
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
	bool stepMode = false;
	bool shiftCompat = false;  // https://chip8.gulrak.net/#quirk5

	void reset();
	void loadSystem();
	void cycle();
	void draw(byte xr, byte yr, byte height);
};
