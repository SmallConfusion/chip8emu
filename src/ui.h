#pragma once
#include <SDL.h>

class UI {
   public:
	void run();
	bool hasImgui();
	bool displayDebug();

   private:
	void mainloop();

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Texture* gameTexture = nullptr;

	bool running = false;
	bool debugVisible = true;
};