#include "sound.h"
#include <SDL.h>
#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <print>
#include "engine/rand.h"

constexpr int FREQUENCY = 48000;

Sound::Sound() {
	SDL_AudioSpec wantSpec, haveSpec;

	SDL_zero(wantSpec);
	wantSpec.freq = FREQUENCY;
	wantSpec.format = AUDIO_S16;
	wantSpec.channels = 1;
	wantSpec.samples = 2048;
	wantSpec.callback = generate;
	wantSpec.userdata = this;

	audioDevice = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, NULL);

	if (audioDevice == 0) {
		std::println("Error opening audio device: {:}", SDL_GetError());
	}

	SDL_PauseAudioDevice(audioDevice, 0);
}

void Sound::showUI() {
	ImGui::Begin("Sound");

	ImGui::LabelText("Sound", playing ? "ON" : "OFF");
	ImGui::InputDouble("Volume", &volume);
	ImGui::InputDouble("Frequency", &frequency);
	ImGui::InputDouble("Duty", &duty);

	ImGui::End();
}

void Sound::generate(void* data, unsigned char* buffer, int length) {
	Sound* s = static_cast<Sound*>(data);

	for (int i = 0; i < length; i += 2) {
		double desiredAmplitude;

		if (s->playing) {
			bool on = std::fmod(s->samplePos, 1 / s->frequency) * s->frequency >
					  s->duty;
			desiredAmplitude = on ? 0.9 : -0.9;
		} else {
			desiredAmplitude = 0;
		}

		s->samplePos += 1.0 / FREQUENCY;

		s->amplitude += 0.8 * (desiredAmplitude - s->amplitude);

		int16_t data = std::round(s->amplitude * s->volume);

		buffer[i] = 0xF0 & data;
		buffer[i + 1] = 0x0F & data;
	}
}
