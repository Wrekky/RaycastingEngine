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

void clearColorBuffer(uint32_t color) {
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

void drawPixel(int x, int y, uint32_t color) {
	colorBuffer[(WINDOW_WIDTH * y) + x] = color;
}