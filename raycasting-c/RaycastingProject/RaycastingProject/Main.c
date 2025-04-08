#include <stdio.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <stdbool.h>

#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "rays.h"
#include "player.h"
#include "wall.h"
bool isGameRunning = false;
int ticksLastFrame = 0;

void setup() {
	loadTextures();
}

void processInput() {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
		case SDL_EVENT_QUIT: {
			isGameRunning = false;
			break;
		}
		case SDL_EVENT_KEY_DOWN: {
			if (event.key.key == SDLK_ESCAPE)
				isGameRunning = false;
			if (event.key.key == SDLK_UP)
				player.walkDirection = 1;
			if (event.key.key == SDLK_DOWN)
				player.walkDirection = -1;
			if (event.key.key == SDLK_LEFT)
				player.turnDirection = -1;
			if (event.key.key == SDLK_RIGHT)
				player.turnDirection = 1;
			break;
		}
		case SDL_EVENT_KEY_UP: {
			if (event.key.key == SDLK_UP)
				player.walkDirection = 0;
			if (event.key.key == SDLK_DOWN)
				player.walkDirection = 0;
			if (event.key.key == SDLK_LEFT)
				player.turnDirection = 0;
			if (event.key.key == SDLK_RIGHT)
				player.turnDirection = 0;
			break;
		}

	}
}

void update() {

	//while (SDL_GetTicks() < ticksLastFrame + FRAME_TIME_LENGTH);
	//SDL delay should be more efficient but potentially inconsistent. Will use while loop if it doesnt feel correct once all game logic is implemented.
	int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticksLastFrame);
	if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
		SDL_Delay(timeToWait);
	}
	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	ticksLastFrame = SDL_GetTicks();

	//Multiply any moving game object by deltaTime.
	movePlayer(deltaTime);
	castAllRays();
}

void render() {	
	clearColorBuffer(0xFFFFFFFF);
	render3DProjection();
	renderMap();
	renderPlayer();
	renderRays();
	renderColorBuffer();
}

void releaseResources() {
	freeTextures();
	destroyWindow();
}

int main(int argc, char* args[]) {
	isGameRunning = initializeWindow();
	setup();
	while (isGameRunning) {
		processInput();
		
		update();
		
		render();
	}
	destroyWindow();
	return 0;
}