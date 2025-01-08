#pragma once
#include <SDL.h>
#include <functional>
#include "engine/engine.h"

class UI {
   public:
	UI(Engine* engine) : engine(engine) {};

	bool step = false;
	bool keymap[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	void run();
	bool hasImgui();

	void debugInfo(std::function<void(void)> info) const;

   private:
	static void mainloop(void* tv);	// should be UI* this
	void drawEngine();

	Engine* engine;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Texture* gameTexture = nullptr;

	bool running = false;
	bool debugVisible = true;

	bool isFullscreen = false;
};