#include <stdio.h>
#include <SDL3/SDL.h>
#include "constants.h"
#include <limits.h>


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

struct Ray {
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;

	int wasHitVertical;
	int wallHitType;
	int isRayFacingUp;
	int isRayFacingDown;
	int isRayFacingLeft;
	int isRayFacingRight;

} rays[NUM_RAYS];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = FALSE;
float playerX, playerY;
int ticksLastFrame = 0;

Uint32* colorBuffer = NULL;

SDL_Texture* colorBufferTexture;

Uint32* wallTexture = NULL;

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
	//Free allocated resources
	free(colorBuffer);
	SDL_DestroyTexture(colorBufferTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
void setup() {
	// TODO:
	// Initialize and setup game objects.

	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.width = 1;
	player.height = 1;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngle = PI / 2; 
	player.walkSpeed = 200;
	player.turnSpeed = 90 * (PI / 180); //90 degrees per second


	//allocated total amount of bytes for colorBuffer.

	colorBuffer = (Uint32*)malloc(sizeof(Uint32) * (Uint32)WINDOW_WIDTH * (Uint32)WINDOW_HEIGHT);

	colorBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	wallTexture = (Uint32*)malloc(sizeof(Uint32) * (Uint32)WALL_TEXTURE_WIDTH * (Uint32)WALL_TEXTURE_HEIGHT);

	wallTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WALL_TEXTURE_WIDTH,
		WALL_TEXTURE_HEIGHT
	);
	//creating a texture, to be subbed out for a png later.
	for (int x = 0; x < WALL_TEXTURE_WIDTH; x++) {
		for (int y = 0; y < WALL_TEXTURE_HEIGHT; y++) {
			wallTexture[(WALL_TEXTURE_HEIGHT * x) + y] = (x % 8 && y % 8) ? 0xFF0000FF : 0xFF000000;
		}
	}

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
	int foundHorzWallHit = FALSE;
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

	while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH && nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT) {
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

		horzHitType = hasWallAt(xToCheck, yToCheck);
		if (horzHitType > 0) {
			horzWallHitX = nextHorzTouchX;
			horzWallHitY = nextHorzTouchY;
			foundHorzWallHit = TRUE;
			break;
		}
		else {
			nextHorzTouchX += xStep;
			nextHorzTouchY += yStep;
		}
	}
	

	//Vertical Ray-grid intersection code
	int foundVertWallHit = FALSE;
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
	while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH && nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) {
		float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
		float yToCheck = nextVertTouchY;

		vertHitType = hasWallAt(xToCheck, yToCheck);
		if (vertHitType > 0) {
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			foundVertWallHit = TRUE;
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
		rays[stripId].wasHitVertical = TRUE;
	}
	else {
		rays[stripId].distance = horzHitDistance;
		rays[stripId].wallHitType = horzHitType;
		rays[stripId].wallHitX = horzWallHitX;
		rays[stripId].wallHitY = horzWallHitY;
		rays[stripId].wasHitVertical = FALSE;
	}

	rays[stripId].rayAngle = rayAngle;
	rays[stripId].isRayFacingDown = isRayFacingDown;
	rays[stripId].isRayFacingUp = isRayFacingUp;
	rays[stripId].isRayFacingLeft = isRayFacingLeft;
	rays[stripId].isRayFacingRight = isRayFacingRight;
}

void castAllRays() {
	float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

	for (int stripId = 0; stripId < NUM_RAYS; stripId++) {
		castRay(rayAngle, stripId);

		rayAngle += FOV_ANGLE / NUM_RAYS;
	}
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

void renderRays() {
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
	castAllRays();
	//Multiply any moving game object by deltaTime.

}

void generate3DProjection() {
	for (int i = 0; i < NUM_RAYS; i++) {
		rays[i];
		float correctedDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle); 
		float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2); //how far the player is from the projection plane.
		float projectedWallHeight = (TILE_SIZE / correctedDistance) * distanceProjPlane;


		int wallStripHeight = (int)projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2); 
		wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel; 

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);//dont need to calculate this..
		wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

		//render wall from wallTopPixel to wallBottomPixel.
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			if (y < wallTopPixel) {
				colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFFADD8E6;
			}
			else if (y >= wallTopPixel && y <= wallBottomPixel) {
				if (rays[i].wasHitVertical) {
					colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFFFFFFFF;
				}
				else {
					colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFFCCCCCC;
				}
			}
			else if (y > wallBottomPixel)
			{
				colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFF000000;
			}


		}


	}
}
void clearColorBuffer(Uint32 color) {
	for (int x = 0; x < WINDOW_WIDTH; x++) {
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			colorBuffer[(WINDOW_WIDTH * y) + x] = color;
		}
	}
}

void renderColorBuffer() {
	SDL_UpdateTexture(colorBufferTexture, 
		NULL, 
		colorBuffer, 
		(int)(Uint32)WINDOW_WIDTH * sizeof(Uint32)
	);
	SDL_RenderTexture(renderer, colorBufferTexture, NULL, NULL);
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	generate3DProjection();
	//clear the color buffer
	renderColorBuffer();
	clearColorBuffer(0xFFBBBBBB);//Passing a color. 0x = hexadecimal, FF = full opacity (256?), R = 00, G = 00, B = 00

	//Minimap display
	renderMap();
	renderRays();
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