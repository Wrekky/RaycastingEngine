#include "wall.h"
void render3DProjection() {
	for (int x = 0; x < NUM_RAYS; x++) {
		rays[x];
		float correctedDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
		float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
		float projectedWallHeight = (TILE_SIZE / correctedDistance) * distanceProjPlane;


		int wallStripHeight = (int)projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);

		int textureOffsetX, textureOffsetY;
		if (rays[x].wasHitVertical) {
			textureOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
		}
		else {
			textureOffsetX = (int)rays[x].wallHitX % TILE_SIZE;
		}

		int textureIndex = rays[x].wallHitType - 1;
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			if (y < wallTopPixel)
			{
				drawPixel(x, y, 0xFFADD8E6);
			}
			else if (y >= wallTopPixel && y <= wallBottomPixel)
			{
				textureOffsetY = (y - wallTopPixel) * ((float)wallTextures[textureIndex].height / wallStripHeight);
				uint32_t texturePixelColor = wallTextures[textureIndex].texture_buffer[(wallTextures[textureIndex].width * textureOffsetY) + textureOffsetX];
				drawPixel(x, y, texturePixelColor);
			}
			else if (y > wallBottomPixel)
			{
				drawPixel(x, y, 0xFF000000);
			}
		}
	}
}