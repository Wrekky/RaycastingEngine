#ifndef MAP_H
#define MAP_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "defs.h"
#include "graphics.h"

#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

int hasWallAt(float x, float y);

int getMapAt();

void renderMap();

bool isInsideMap(float x, float y);
#endif