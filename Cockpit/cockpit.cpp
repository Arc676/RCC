#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <getopt.h>

#include <cstddef>
#include <iostream>

#include "dashboard.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#define FRAME_DELAY 1000 / 60

void initializeUI(SDL_Window* const window, SDL_Renderer* const renderer) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
}

void newFrame() {
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void printHelp() {
	std::cout
		<< "Options:\n"
		<< "\t-w|--windowed: launch in windowed mode instead of fullscreen\n"
		<< "\t-h|--help: show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Failed to initialize SDL: " << SDL_GetError()
				  << std::endl;
		return 1;
	}

	constexpr int WINDOW_WIDTH  = 1000;
	constexpr int WINDOW_HEIGHT = 1000;

	unsigned winFlags = SDL_WINDOW_FULLSCREEN;

	static const char* shortOpts    = "wh";
	static struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},
		{"windowed", no_argument, NULL, 'w'},
		{0, 0, 0, 0},
	};

	while (true) {
		int idx = 0;
		// NOLINTNEXTLINE(concurrency-mt-unsafe)
		int opt = getopt_long(argc, argv, shortOpts, longOpts, &idx);

		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 'h':
				printHelp();
				return 0;
			case 'w':
				winFlags = SDL_WINDOW_MAXIMIZED;
				break;
			default:
				return 1;
		}
	}

	SDL_Window* win = SDL_CreateWindow(
		"Open RC Cockpit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, winFlags);

	unsigned flags         = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, flags);
	initializeUI(win, renderer);

	Dashboard dashboard;

	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			dashboard.handleEvent(&event);
		}

		newFrame();
		if (dashboard.drawCockpitUI()) {
			break;
		}
		ImGui::Render();

		// NOLINTNEXTLINE(readability-magic-numbers)
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
		SDL_Delay(FRAME_DELAY);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
