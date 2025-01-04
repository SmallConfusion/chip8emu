#pragma once
#include <bitset>
#include <cstdint>
#include <queue>
#include <vector>

typedef uint8_t byte;
typedef uint16_t addr;

class Engine {
	friend class Debug;

   public:
	Engine();

	void loadROM(const char* filename);
	void update();

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
};
