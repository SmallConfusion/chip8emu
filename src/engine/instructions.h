#pragma once
#include "engine/engineTypes.h"

constexpr inst I_CLEAR_SCREEN = 0x00E0;

constexpr inst I_JUMP = 0x1000;
constexpr inst I_JUMP_CHECK = 0xF000;
constexpr inst I_JUMP_MASK = 0x0FFF;

constexpr inst I_SET_REG = 0x6000;
constexpr inst I_SET_REG_CHECK = 0xF000;
constexpr inst I_SET_REG_REG_MASK = 0x0F00;
constexpr byte I_SET_REG_REG_SHIFT = 8;
constexpr inst I_SET_REG_VALUE_MASK = 0x00FF;

constexpr inst I_ADD_REG = 0x7000;
constexpr inst I_ADD_REG_CHECK = 0xF000;
constexpr inst I_ADD_REG_REG_MASK = 0x0F00;
constexpr byte I_ADD_REG_REG_SHIFT = 8;
constexpr inst I_ADD_REG_VALUE_MASK = 0x00FF;

constexpr inst I_SET_INDEX = 0xA000;
constexpr inst I_SET_INDEX_CHECK = 0xF000;
constexpr inst I_SET_INDEX_MASK = 0x0FFF;

constexpr inst I_DRAW = 0xD000;
constexpr inst I_DRAW_CHECK = 0xF000;
constexpr inst I_DRAW_X_MASK = 0x0F00;
constexpr byte I_DRAW_X_SHIFT = 8;
constexpr inst I_DRAW_Y_MASK = 0x00F0;
constexpr byte I_DRAW_Y_SHIFT = 4;
constexpr inst I_DRAW_HEIGHT_MASK = 0x000F;

