#pragma once
#include <SDL.h>

class Sound {
   public:
	bool playing = false;

	Sound();

	void showUI();

   private:
	SDL_AudioDeviceID audioDevice;
	double samplePos = 0;
	double amplitude = 0;

	double volume = 50.0;
	double frequency = 500.0;
	double duty = 0.5;

	static void generate(void* data, unsigned char* buffer, int length);
};
