#include "sprite.h"

#define NUM_SPRITES 1

static sprite_t sprites[NUM_SPRITES] = {
	{.x = 640, .y = 630, .textureIndex = 13}
};

void renderMapSprites() {
	for (int i = 0; i < NUM_SPRITES; i++) {
		drawRect(2, 2, sprites[i].x * MINIMAP_SCALE_FACTOR, sprites[i].y * MINIMAP_SCALE_FACTOR, sprites[i].isVisible ? BLUE : GREEN);
	}
}

void renderSpriteProjection() {
	sprite_t visibleSprites[NUM_SPRITES];
	int numVisibleSprites = 0;

	for (int i = 0; i < NUM_SPRITES; i++) {
		float angleSpritePlayer = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);

		if (angleSpritePlayer > PI) {
			angleSpritePlayer -= TWO_PI;
		}
		if (angleSpritePlayer < -PI) {
			angleSpritePlayer += TWO_PI;
		}
		angleSpritePlayer = fabs(angleSpritePlayer);
		if (angleSpritePlayer < (FOV_ANGLE / 2)) {
			sprites[i].isVisible = true;
			visibleSprites[numVisibleSprites] = sprites[i];
			numVisibleSprites++;
		}
		else {
			sprites[i].isVisible = false;
		}
	}
}