#include "sprite.h"
#include "graphics.h"
#include "textures.h"
#include "player.h"
#include "defs.h"
#include "color.h"
#define NUM_SPRITES 1

static sprite_t sprites[NUM_SPRITES] = {
	{.x = 640, .y = 630, .textureIndex = 13}
};

void renderMapSprites() {
	for (int i = 0; i < NUM_SPRITES; i++) {
		drawRect(2, 2, sprites[i].x * MINIMAP_SCALE_FACTOR, sprites[i].y * MINIMAP_SCALE_FACTOR, BLUE);
	}
}

void renderSpriteProjection() {

}