#pragma once
#include <SDL.h>
#include "engine/engine.h"
#include <functional>

class UI {
   public:
	UI(Engine engine) : engine(engine) {};

	bool step = false;

	void run();
	bool hasImgui();

	void debugInfo(std::function<void(void)> info) const;


   private:
	void mainloop();
	void drawEngine();

	Engine engine;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Texture* gameTexture = nullptr;

	bool running = false;
	bool debugVisible = true;
};