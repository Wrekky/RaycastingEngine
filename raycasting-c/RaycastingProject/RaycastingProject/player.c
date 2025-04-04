#include "player.h"

player_t player = {
	.x = MAP_NUM_COLS * TILE_SIZE / 2,
	.y = MAP_NUM_ROWS * TILE_SIZE / 2,
	.width = 1,
	.height = 1,
	.turnDirection = 0,
	.walkDirection = 0,
	.rotationAngle = PI / 2,
	.walkSpeed = 200,
	.turnSpeed = 90 * (PI / 180), //90 degrees per second
};

void movePlayer(float deltaTime) {
	player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
	int moveStep = player.walkSpeed * player.walkDirection * deltaTime;
	float newPlayerX = (player.x + (cos(player.rotationAngle) * moveStep));
	float newPlayerY = (player.y + (sin(player.rotationAngle) * moveStep));
	if (hasWallAt(newPlayerX, newPlayerY) == 0) {
		player.x = newPlayerX;
		player.y = newPlayerY;
	}
}

void renderPlayer() {
	drawRect(player.width * MINIMAP_SCALE_FACTOR, 
		player.height * MINIMAP_SCALE_FACTOR, 
		player.x * MINIMAP_SCALE_FACTOR, 
		player.y * MINIMAP_SCALE_FACTOR, 
		0xFFFF0000);
}