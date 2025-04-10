#ifndef TEXTURES_H
#define TEXTURES_H
#include <stdint.h>
#include "defs.h"
#include "upng.h"

#define NUM_TEXTURES 9

upng_t* textures[NUM_TEXTURES];

void loadTextures();

void freeTextures();

#endif