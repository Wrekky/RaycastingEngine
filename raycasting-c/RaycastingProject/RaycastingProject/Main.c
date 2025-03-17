#include <stdio.h>
#include <SDL3/SDL.h>
#include "constants.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
	float x;
	float y;
	float width;
	float height;
	int turnDirection; //-1 left, +1 right
	int walkDirection; //-1 backwards, += forwards
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player;

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

	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.width = 5;
	player.height = 5;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngle = PI / 2; //90 degrees
	player.walkSpeed = 100;
	player.turnSpeed = 45 * (PI / 180); //45 degrees per second


}

int hasWallAt(float x, float y) {
	if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
		return TRUE;
	}
	int mapGridIndexX = floor(x / TILE_SIZE);
	int mapGridIndexY = floor(y / TILE_SIZE);
	return map[mapGridIndexY][mapGridIndexX];
}
void movePlayer(float deltaTime) {
	player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
	int moveStep = player.walkSpeed * player.walkDirection * deltaTime;
	float newPlayerX = (player.x + (cos(player.rotationAngle) * moveStep));
	float newPlayerY = (player.y + (sin(player.rotationAngle) * moveStep));
	//wall collision
	//Can do two checks for player x with changed y and changed x with player Y.
	//If I do two checks the walls you wont run into edge case movements where you can be a pixel or so away from a wall.
	if (hasWallAt(newPlayerX, newPlayerY) == 0) {
		player.x = newPlayerX;
		player.y = newPlayerY;
	}
}
void renderPlayer() {
	SDL_FRect playerRect = {
		MINIMAP_SCALE_FACTOR * player.x,
		MINIMAP_SCALE_FACTOR * player.y,
		MINIMAP_SCALE_FACTOR * player.width,
		MINIMAP_SCALE_FACTOR * player.height
	};
	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderFillRect(renderer, &playerRect);

	//Look direction rendering
	SDL_RenderLine(
		renderer,
		MINIMAP_SCALE_FACTOR * player.x,
		MINIMAP_SCALE_FACTOR * player.y,
		MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
		MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40
		);
}
void renderMap() {
	for (int i = 0; i < MAP_NUM_ROWS; i++) {
		for (int j = 0; j < MAP_NUM_COLS; j++) {
			int tileX = j * TILE_SIZE;
			int tileY = i * TILE_SIZE;
			if (map[i][j] > 0) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			}
			SDL_FRect rect = { tileX * MINIMAP_SCALE_FACTOR, tileY * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR };
			SDL_RenderFillRect(renderer, &rect);
		}
	}
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


	movePlayer(deltaTime);
	//Multiply any moving game object by deltaTime.

}
void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	//SDL_FRect rect = { playerX, playerY, 20, 20 };
	renderMap();
	//renderRays();
	renderPlayer();

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