#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <stdio.h>

#include "defs.h"

bool initializeWindow();

void destroyWindow();

void clearColorBuffer(uint32_t color);

void renderColorBuffer();

void drawPixel(int x, int y, uint32_t color);

#endif