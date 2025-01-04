#pragma once
#include <bitset>
#include <cstdint>
#include <queue>
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

	std::queue<addr> stack;

   private:
	void reset();
	void loadSystem();
	void cycle();
	void draw(byte xr, byte yr, byte height);
};
