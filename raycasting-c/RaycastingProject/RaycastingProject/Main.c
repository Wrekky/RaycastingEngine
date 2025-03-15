#include <stdio.h>
#include <SDL3/SDL.h>
#include "constants.h"
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = FALSE;
float playerX, playerY;
int ticksLastFrame = 0;
int initializedWindow() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	if (!window) {
		fprintf(stderr, "Error creating SDL window. \n");
		return FALSE;
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return TRUE;
}

void destroyWindow() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
void setup() {
	// TODO:
	// Initialize and setup game objects.

	playerX = 0;
	playerY = 0;

}
void processInput() {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
		case SDL_EVENT_QUIT: {
			isGameRunning = FALSE;
			break;
		}
		case SDL_EVENT_KEY_DOWN: {
			if (event.key.key == SDLK_ESCAPE)
				isGameRunning = false;
		}

	}
}

void update() {

	while (SDL_GetTicks() < ticksLastFrame + FRAME_TIME_LENGTH);

	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	ticksLastFrame = SDL_GetTicks();

	playerX += 20 * deltaTime;
	playerY += 20 * deltaTime;

}
void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_FRect rect = { playerX, playerY, 20, 20 };
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);
}
int main(int argc, char* args[]) {
	isGameRunning = initializedWindow();
	setup();
	while (isGameRunning) {
		processInput();
		update();
		
		render();
	}

	destroyWindow();

	return 0;
}