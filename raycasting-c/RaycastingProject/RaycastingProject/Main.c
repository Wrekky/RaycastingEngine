#include <stdio.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <limits.h>
#include <stdbool.h>

#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"

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

struct Ray {
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;

	int wallHitType;

	bool wasHitVertical;

} rays[NUM_RAYS];


bool isGameRunning = false;
float playerX, playerY;
int ticksLastFrame = 0;


void setup() {
	// TODO:
	// Initialize and setup game objects.

	player.x = MAP_NUM_COLS * TILE_SIZE / 2;
	player.y = MAP_NUM_ROWS * TILE_SIZE / 2;
	player.width = 1;
	player.height = 1;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngle = PI / 2; 
	player.walkSpeed = 200;
	player.turnSpeed = 90 * (PI / 180); //90 degrees per second
	loadWallTextures();
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
	/*SDL_FRect playerRect = {
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
	*/
}

float normalizeAngle(float angle) {
	angle = remainder(angle, TWO_PI);
	if (angle < 0) {
		angle = TWO_PI + angle;
	}
	return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int stripId) {
	rayAngle = normalizeAngle(rayAngle);

	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;

	int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
	int isRayFacingLeft = !isRayFacingRight;
	float xIntercept, yIntercept;
	float xStep, yStep;

	//Horizontal Ray-grid intersection code
	bool foundHorzWallHit = false;
	float horzWallHitX = 0;
	float horzWallHitY = 0;
	int horzHitType = 0;

	yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yIntercept += isRayFacingDown ? TILE_SIZE : 0;

	xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

	yStep = TILE_SIZE;
	yStep *= isRayFacingUp ? -1 : 1;

	xStep = TILE_SIZE / tan(rayAngle);
	xStep *= (isRayFacingLeft && xStep > 0) ? -1 : 1;
	xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;
	////allsame
	float nextHorzTouchX = xIntercept;
	float nextHorzTouchY = yIntercept;

	//while (nextHorzTouchX >= 0 && nextHorzTouchX <= MAP_NUM_COLS * TILE_SIZE && nextHorzTouchY >= 0 && nextHorzTouchY <= MAP_NUM_ROWS * TILE_SIZE) {
	while (isInsideMap(nextHorzTouchX, nextHorzTouchY)) {
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

		horzHitType = hasWallAt(xToCheck, yToCheck);
		if (horzHitType > 0) {
			horzWallHitX = nextHorzTouchX;
			horzWallHitY = nextHorzTouchY;
			foundHorzWallHit = true;
			break;
		}
		else {
			nextHorzTouchX += xStep;
			nextHorzTouchY += yStep;
		}
	}
	

	//Vertical Ray-grid intersection code
	bool foundVertWallHit = false;
	float vertWallHitX = 0;
	float vertWallHitY = 0;
	int vertHitType = 0;
	xIntercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xIntercept += isRayFacingRight ? TILE_SIZE : 0;

	yIntercept = player.y + (xIntercept - player.x) * tan(rayAngle);

	xStep = TILE_SIZE;
	xStep *= isRayFacingLeft ? -1 : 1;

	yStep = TILE_SIZE * tan(rayAngle);
	yStep *= (isRayFacingUp && yStep > 0) ? -1 : 1;
	yStep *= (isRayFacingDown && yStep < 0) ? -1 : 1;

	float nextVertTouchX = xIntercept;
	float nextVertTouchY = yIntercept;
	while (isInsideMap(nextVertTouchX, nextVertTouchY)) {
		float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
		float yToCheck = nextVertTouchY;

		vertHitType = hasWallAt(xToCheck, yToCheck);
		if (vertHitType > 0) {
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			foundVertWallHit = true;
			break;
		}
		else {
			nextVertTouchX += xStep;
			nextVertTouchY += yStep;
		}
	}

	//calculate vertical and horizontal distances & pick the shortest one.
	float horzHitDistance = foundHorzWallHit 
		? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
		: INT_MAX;
	float vertHitDistance = foundVertWallHit 
		? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
		: INT_MAX;
	
	if (vertHitDistance < horzHitDistance) {
		rays[stripId].distance = vertHitDistance;
		rays[stripId].wallHitType = vertHitType;
		rays[stripId].wallHitX = vertWallHitX;
		rays[stripId].wallHitY = vertWallHitY;
		rays[stripId].wasHitVertical = true;
		rays[stripId].rayAngle = rayAngle;
	}
	else {
		rays[stripId].distance = horzHitDistance;
		rays[stripId].wallHitType = horzHitType;
		rays[stripId].wallHitX = horzWallHitX;
		rays[stripId].wallHitY = horzWallHitY;
		rays[stripId].wasHitVertical = false;
		rays[stripId].rayAngle = rayAngle;
	}
}

void castAllRays() {
	float projectionPlaneDist = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
	for (int stripId = 0; stripId < NUM_RAYS; stripId++) {
		
		float rayAngle = player.rotationAngle + atan((stripId - NUM_RAYS / 2) / projectionPlaneDist);
		castRay(rayAngle, stripId);
	}
}

void renderRays() {
	/*
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int i = 0; i < NUM_RAYS; i++) {
		SDL_RenderLine(
			renderer,
			MINIMAP_SCALE_FACTOR * player.x,
			MINIMAP_SCALE_FACTOR * player.y,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitY
		);
	}
	*/
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

	
	movePlayer(deltaTime);
	castAllRays();
	//Multiply any moving game object by deltaTime.

}

void render3DProjection() {
	for (int x = 0; x < NUM_RAYS; x++) {
		rays[x];
		float correctedDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle); 
		float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2); //how far the player is from the projection plane.
		float projectedWallHeight = (TILE_SIZE / correctedDistance) * distanceProjPlane;


		int wallStripHeight = (int)projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2); 

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);//dont need to calculate this..

		//render wall from wallTopPixel to wallBottomPixel.
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
	render3DProjection();
	//clear the color buffer
	renderMap();
	renderColorBuffer();
	clearColorBuffer(0xFFBBBBBB);//Passing a color. 0x = hexadecimal, FF = full opacity (256?), R = 00, G = 00, B = 00

	//Minimap display
	
	renderRays();
	renderPlayer();
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