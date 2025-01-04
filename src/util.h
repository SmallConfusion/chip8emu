#pragma once
#include <cstdint>
#include <format>
#include "engine/engine.h"

struct util {
	static char nibbleToHex(byte b) {
		if (b < 10) {
			return '0' + b;
		} else {
			return 'A' + b - 10;
		}
	}

	static std::string byteToHex(byte b) {
		return std::format("{}{}", nibbleToHex((b & 0xF0u) >> 4),
						   nibbleToHex(b & 0x0Fu));
	}

	static std::string addrToHex(addr a) {
		return std::format("{}{}{}", nibbleToHex((a & 0xF00u) >> 8),
						   nibbleToHex((a & 0x0F0u) >> 4), nibbleToHex(a & 0x00Fu));
	}

	static std::string instructionToHex(inst a) {
		return std::format("{}{}{}{}", nibbleToHex((a & 0xF000u) >> 12),
						   nibbleToHex((a & 0x0F00u) >> 8),
						   nibbleToHex((a & 0x00F0u) >> 4),
						   nibbleToHex(a & 0x000Fu));
	}
};
