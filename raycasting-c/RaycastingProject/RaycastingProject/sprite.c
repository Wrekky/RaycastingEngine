#include "sprite.h"

#define NUM_SPRITES 4

static sprite_t sprites[NUM_SPRITES] = {
	{.x = 640, .y = 630, .textureIndex = 10},
	{ .x = 300, .y = 300, .textureIndex = 11 },
	{.x = 200, .y = 200, .textureIndex = 12},
	{.x = 400, .y = 400, .textureIndex = 13}
};

//Sprite sorter using bubble sort
void spriteSorter(sprite_t visibleSprites[], int numVisibleSprites) {
	sprite_t tempObj;
	bool noSwaps = false;
	while (!noSwaps) {
		noSwaps = true;
		for (int i = 0; i < numVisibleSprites; i++) {
			if (i + 1 < numVisibleSprites) {
				if (visibleSprites[i].distance < visibleSprites[i + 1].distance) {
					tempObj = visibleSprites[i];
					visibleSprites[i] = visibleSprites[i + 1];
					visibleSprites[i + 1] = tempObj;
					noSwaps = false;
				}
			}
		}
	}
}
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
		if (angleSpritePlayer < (FOV_ANGLE / 2) + (FOV_ANGLE / 4)) {
			sprites[i].isVisible = true;
			sprites[i].angle = angleSpritePlayer;
			sprites[i].distance = distanceBetweenPoints(sprites[i].x, sprites[i].y, player.x, player.y);
			visibleSprites[numVisibleSprites] = sprites[i];
			numVisibleSprites++;
		}
		else {
			sprites[i].isVisible = false;
		}
	}
	//sort numVisibleSprites
	//sorting --> got to be pass and return?
	
	spriteSorter(visibleSprites, numVisibleSprites);
	for (int i = 0; i < numVisibleSprites; i++) {
		printf("%f", visibleSprites[i].distance);
		printf(i + "\n");
	}
	
	for (int i = 0; i < numVisibleSprites; i++) {
		sprite_t sprite = visibleSprites[i];

		float projectedSpriteHeight = (TILE_SIZE / sprite.distance) * DIST_PROJ_PLANE;
		float projectedSpriteWidth = projectedSpriteHeight;
		//Top sprite pixel
		int spriteTopPixel = (WINDOW_HEIGHT / 2) - (projectedSpriteHeight / 2);
		spriteTopPixel = (spriteTopPixel < 0) ? 0 : spriteTopPixel;

		//Bottom sprite pixel
		int spriteBottomPixel = (WINDOW_HEIGHT / 2) + (projectedSpriteHeight / 2);
		spriteBottomPixel = (spriteBottomPixel > WINDOW_HEIGHT) ? WINDOW_HEIGHT : spriteBottomPixel;

		float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
		float spriteScreenPosX = tan(spriteAngle) * DIST_PROJ_PLANE;
		//Sprite left pixel
		int spriteLeftPixel = (WINDOW_WIDTH / 2) + spriteScreenPosX;
		//centering the sprite
		spriteLeftPixel -= projectedSpriteWidth / 2;
		//Sprite right pixel
		int spriteRightPixel = spriteLeftPixel + projectedSpriteWidth;

		//find the width and height of the visible sprite texture
		int textureWidth = upng_get_width(textures[sprite.textureIndex]);
		int textureHeight = upng_get_height(textures[sprite.textureIndex]);

		for (int x = spriteLeftPixel; x < spriteRightPixel; x++) {
			float texturePixelWidth = textureWidth / projectedSpriteWidth;
			float texturePixelHeight = textureHeight / projectedSpriteHeight;
			int textureOffsetX = (x - spriteLeftPixel) * texturePixelWidth;
			for (int y = spriteTopPixel; y < spriteBottomPixel; y++) {
				int textureOffsetY = (y - spriteTopPixel) * texturePixelHeight;
				if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT) {
					color_t* textureBuffer = (color_t*)upng_get_buffer(textures[sprite.textureIndex]);
					color_t texturePixelColor = textureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];
					if (texturePixelColor != 0xFFFF00FF) {
						drawPixel(x, y, texturePixelColor);
					}
				}
			}
		}
	}
}