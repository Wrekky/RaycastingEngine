#include "graphics.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static uint32_t* colorBuffer = NULL;
static SDL_Texture* colorBufferTexture;

bool initializeWindow() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	if (!window) {
		fprintf(stderr, "Error creating SDL window. \n");
		return false;
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	//allocated total amount of bytes for colorBuffer.

	colorBuffer = (uint32_t*)malloc(sizeof(uint32_t) * (uint32_t)WINDOW_WIDTH * (uint32_t)WINDOW_HEIGHT);

	colorBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	return true;
}

void destroyWindow() {
	//Free allocated resources
	free(colorBuffer);
	SDL_DestroyTexture(colorBufferTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void clearColorBuffer(color_t color) {
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
		(int)(uint32_t)WINDOW_WIDTH * sizeof(uint32_t)
	);
	SDL_RenderTexture(renderer, colorBufferTexture, NULL, NULL);

	SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y, color_t color) {
	colorBuffer[(WINDOW_WIDTH * y) + x] = color;
}

void drawRect(int width, int height, int x, int y, color_t color) {
	for (int xPos = 0; xPos <= width; xPos++) {
		for (int yPos = 0; yPos <= height; yPos++) {
			drawPixel(xPos + x, yPos + y, color);
		}
	}
}

void drawLine(int x1, int y1, int x2, int y2, color_t color) {
	int deltaX = (x2 - x1);
	int deltaY = (y2 - y1);

	int sideLength = abs(deltaX) >= abs(deltaY) ? abs(deltaX) : abs(deltaY);

	float xIncline = deltaX / (float)sideLength;
	float yInclude = deltaY / (float)sideLength;

	float currentX = x1;
	float currentY = y1;
	for (int i = 0; i < sideLength; i++) {
		drawPixel(round(currentX), round(currentY), color);
		currentX += xIncline;
		currentY += yInclude;
	}
}