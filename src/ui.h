#pragma once
#include <SDL.h>

class Engine;

class UI {
   public:
	UI(Engine* engine) : engine(engine) {};

	void run();
	bool hasImgui();

   private:
	void mainloop();

	Engine* engine;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Texture* gameTexture = nullptr;

	bool running = false;
	bool debugVisible = true;
};