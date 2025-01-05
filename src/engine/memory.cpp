#include "memory.h"
#include <vector>
#include "engine/engineTypes.h"

void Memory::resize(int size) {
	data.resize(size, 0);
}

std::vector<byte>::iterator Memory::begin() {
	return data.begin();
}

std::vector<byte>::iterator Memory::end() {
	return data.end();
}

byte& Memory::operator[](int i) {
	return data[i % data.size()];
}

const byte& Memory::operator[](int i) const {
	return data[i % data.size()];
}
