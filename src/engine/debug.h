#pragma once
#include "engine.h"


class Debug : public Engine {
   public:
	static void show(const Engine& engine);

   private:
	static void showRam(const Engine& engine);
	static void showRegs(const Engine& engine);
	static void showProgram(const Engine& engine);
};