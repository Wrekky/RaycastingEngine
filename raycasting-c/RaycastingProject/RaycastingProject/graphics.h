#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <stdio.h>

#include "defs.h"

bool initializeWindow();

void destroyWindow();

void clearColorBuffer(color_t color);

void renderColorBuffer();

void drawPixel(int x, int y, color_t color);

void drawRect(int width, int height, int x, int y, color_t color);

void drawLine(int p1x, int p1y, int p2x, int p2y, color_t color);

#endif