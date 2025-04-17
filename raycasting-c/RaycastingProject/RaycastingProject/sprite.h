#ifndef SPRITE_H	
#define SRITE_H
#include "graphics.h"
#include "textures.h"
#include "player.h"
#include "defs.h"
#include "color.h"
#include "utils.h"
typedef struct {
	float x;
	float y;
	float distance;
	float angle;
	int textureIndex;
	bool isVisible;
} sprite_t;

void renderSpriteProjection();
void renderMapSprites();

#endif