#pragma once
#include <cstdint>
#include <format>
#include "engine/engine.h"

struct util {
	static char bitsToHex(byte b) {
		if (b < 10) {
			return '0' + b;
		} else {
			return 'A' + b - 10;
		}
	}

	static std::string byteToHex(byte b) {
		return std::format("{}{}", bitsToHex((b & 0xF0u) >> 4),
						   bitsToHex(b & 0x0Fu));
	}

	static std::string addrToHex(addr a) {
		return std::format("{}{}{}", bitsToHex((a & 0xF00u) >> 8),
						   bitsToHex((a & 0x0F0u) >> 4), bitsToHex(a & 0x00Fu));
	}

	static std::string instructionToHex(addr a) {
		return std::format("{}{}{}{}", bitsToHex((a & 0xF000u) >> 12),
						   bitsToHex((a & 0x0F00u) >> 8),
						   bitsToHex((a & 0x00F0u) >> 4),
						   bitsToHex(a & 0x000Fu));
	}
};
