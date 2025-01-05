#pragma once
#include <vector>
#include "engine/engineTypes.h"

class Memory {
   public:
	void resize(int size);

	std::vector<byte>::iterator begin();
	std::vector<byte>::iterator end();

	byte& operator[](int i);
	const byte& operator[](int i) const;

   private:
	std::vector<byte> data;
};