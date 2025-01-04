#include "ui.h"
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <print>
#include "engine/debug.h"
#include "engine/engine.h"

void UI::run() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, 1280, 720,
							  SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	if (!window) {
		std::println("Failed to create window: %s\n", SDL_GetError());
	}

	renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	if (!renderer) {
		std::println("Failed to create renderer: %s\n", SDL_GetError());
	}

	gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
									SDL_TEXTUREACCESS_STREAMING, 64, 32);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsLight();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	running = true;

	while (running) {
		mainloop();
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool UI::hasImgui() {
	return true;
}

void UI::debugInfo(std::function<void(void)> info) const {
	if (debugVisible) {
		info();
	}
}

void UI::mainloop() {
	step = false;

	SDL_Event ev;
	while (SDL_PollEvent(&ev) != 0) {
		ImGui_ImplSDL2_ProcessEvent(&ev);

		switch (ev.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (ev.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					debugVisible = !debugVisible;
				} else if (ev.key.keysym.scancode == SDL_SCANCODE_F10) {
					step = true;
				}
				break;
		}
	}

	bool isRunning = true;

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	SDL_RenderClear(renderer);

	engine.update(*this);
	drawEngine();

	if (debugVisible) {
		Debug::show(engine);
	}

	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

	SDL_RenderPresent(renderer);
}

void UI::drawEngine() {
	constexpr uint32_t FOREGROUND = 0xFFFFFFFF;
	constexpr uint32_t BACKGROUND = 0xFF000000;

	std::bitset<32 * 64> screen = engine.getDisplay();

	uint32_t* pixels;
	int pitch;

	SDL_LockTexture(gameTexture, nullptr, (void**)&pixels, &pitch);

	for (int i = 0; i < 32 * 64; i++) {
		if (screen.test(i)) {
			pixels[i] = FOREGROUND;
		} else {
			pixels[i] = BACKGROUND;
		}
	}

	SDL_UnlockTexture(gameTexture);

	SDL_RenderCopy(renderer, gameTexture, nullptr, nullptr);
}
