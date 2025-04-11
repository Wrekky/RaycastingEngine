#ifndef SPRITE_H	
#define SRITE_H

typedef struct {
	float x;
	float y;
	float distance;
	float angle;
	int textureIndex;
} sprite_t;

void renderSpriteProjection();
void renderMapSprites();

#endif