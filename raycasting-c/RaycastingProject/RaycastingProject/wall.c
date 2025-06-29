#include "wall.h"
#include "color.h"
// Change the color intensity based on a factor value between 0 and 1
void changeColorIntensity(color_t* color, float factor) {
	color_t a = (*color & 0xFF000000);
	color_t r = (*color & 0x00FF0000) * factor;
	color_t g = (*color & 0x0000FF00) * factor;
	color_t b = (*color & 0x000000FF) * factor;

	*color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

void render3DProjection() {
	for (int x = 0; x < NUM_RAYS; x++) {
		rays[x];
		float correctedDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
		float distanceProjPlane = DIST_PROJ_PLANE;//TODO: replace instances with DIST_PROJ_PLANE.
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

		int textureWidth = upng_get_width(textures[textureIndex]);
		int textureHeight = upng_get_height(textures[textureIndex]);
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			if (y < wallTopPixel)
			{
				drawPixel(x, y, CREAM_YELLOW);
			}
			else if (y > wallTopPixel && y < wallBottomPixel)
			{
				textureOffsetY = (y - wallTopPixel) * ((float)textureHeight / wallStripHeight);
				color_t* textureBuffer = (color_t*)upng_get_buffer(textures[textureIndex]);
				color_t texturePixelColor = textureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];
				
				//make darker based on distance
				float intensity = 1 - (rays[x].distance / (WINDOW_WIDTH + WINDOW_HEIGHT));
				
				if (rays[x].distance > WINDOW_WIDTH + WINDOW_HEIGHT) {
					intensity = 0;
				}
				//distance of wall
				changeColorIntensity(&texturePixelColor, intensity);

				//make darker if wall hits a vertical intersection
				if (rays[x].wasHitVertical) {
					changeColorIntensity(&texturePixelColor, 0.7);
				}
				drawPixel(x, y, texturePixelColor);
			}
			else if (y >= wallBottomPixel)
			{
				drawPixel(x, y, BLACK);
			}
		}
	}
}