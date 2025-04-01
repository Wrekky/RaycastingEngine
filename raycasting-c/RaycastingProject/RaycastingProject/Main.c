#include <stdio.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <limits.h>
#include <stdbool.h>

#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "rays.h"
#include "player.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup() {
	// TODO:
	// Initialize and setup game objects.
	loadWallTextures();
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

void render3DProjection() {
	for (int x = 0; x < NUM_RAYS; x++) {
		rays[x];
		float correctedDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle); 
		float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
		float projectedWallHeight = (TILE_SIZE / correctedDistance) * distanceProjPlane;


		int wallStripHeight = (int)projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2); 

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);

		int textureOffsetX, textureOffsetY;
		if (rays[x].wasHitVertical) {
			textureOffsetX = (int) rays[x].wallHitY % TILE_SIZE;
		}
		else {
			textureOffsetX = (int) rays[x].wallHitX % TILE_SIZE;
		}

		int textureIndex = rays[x].wallHitType - 1;
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			if (y < wallTopPixel) 
			{
				drawPixel(x, y, 0xFFADD8E6);
			}
			else if (y >= wallTopPixel && y <= wallBottomPixel) 
			{
				textureOffsetY = (y - wallTopPixel) * ((float)wallTextures[textureIndex].height / wallStripHeight);
				uint32_t texturePixelColor = wallTextures[textureIndex].texture_buffer[(wallTextures[textureIndex].width * textureOffsetY) + textureOffsetX];
				drawPixel(x, y, texturePixelColor);
			}
			else if (y > wallBottomPixel)
			{
				drawPixel(x, y, 0xFF000000);
			}
		}
	}
}

void render() {	
	clearColorBuffer(0xFFFFFFFF);
	render3DProjection();
	renderMap();
	renderPlayer();
	renderColorBuffer();
	//Passing a color. 0x = hexadecimal, FF = full opacity (256?), R = 00, G = 00, B = 00

	//TODO: Move the following functions to seperate .h & .c files
	renderRays();
	
}

void releaseResources() {
	freeWallTextures();
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