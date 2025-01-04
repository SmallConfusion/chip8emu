#pragma once
#include "engine.h"


class Debug : public Engine {
   public:
	static void show(Engine* engine);

   private:
	static void showRam(Engine* engine);
	static void showRegs(Engine* engine);
	static void showProgram(Engine* engine);
};