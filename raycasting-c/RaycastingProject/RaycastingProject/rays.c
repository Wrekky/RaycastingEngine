#include "rays.h"
#include "color.h"
ray_t rays[NUM_RAYS];

bool isRayFacingDown(float angle) {
	return angle > 0 && angle < PI;
}
bool isRayFacingUp(float angle) {
	return !isRayFacingDown(angle);
}

bool isRayFacingRight(float angle) {
	return angle < 0.5 * PI || angle > 1.5 * PI;
}
bool isRayFacingLeft(float angle) {
	return !isRayFacingRight(angle);
}

void castRay(float rayAngle, int stripId) {
	normalizeAngle(&rayAngle);
	float xIntercept, yIntercept;
	float xStep, yStep;

	//Horizontal Ray-grid intersection code
	bool foundHorzWallHit = false;
	float horzWallHitX = 0;
	float horzWallHitY = 0;
	int horzHitType = 0;

	yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yIntercept += isRayFacingDown(rayAngle) ? TILE_SIZE : 0;

	xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

	yStep = TILE_SIZE;
	yStep *= isRayFacingUp(rayAngle) ? -1 : 1;

	xStep = TILE_SIZE / tan(rayAngle);
	xStep *= (isRayFacingLeft(rayAngle) && xStep > 0) ? -1 : 1;
	xStep *= (isRayFacingRight(rayAngle) && xStep < 0) ? -1 : 1;
	float nextHorzTouchX = xIntercept;
	float nextHorzTouchY = yIntercept;

	while (isInsideMap(nextHorzTouchX, nextHorzTouchY)) {
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp(rayAngle) ? -1 : 0);

		horzHitType = hasWallAt(xToCheck, yToCheck);
		if (horzHitType > 0) {
			horzWallHitX = nextHorzTouchX;
			horzWallHitY = nextHorzTouchY;
			foundHorzWallHit = true;
			break;
		}
		else {
			nextHorzTouchX += xStep;
			nextHorzTouchY += yStep;
		}
	}


	//Vertical Ray-grid intersection code
	bool foundVertWallHit = false;
	float vertWallHitX = 0;
	float vertWallHitY = 0;
	int vertHitType = 0;
	xIntercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xIntercept += isRayFacingRight(rayAngle) ? TILE_SIZE : 0;

	yIntercept = player.y + (xIntercept - player.x) * tan(rayAngle);

	xStep = TILE_SIZE;
	xStep *= isRayFacingLeft(rayAngle) ? -1 : 1;

	yStep = TILE_SIZE * tan(rayAngle);
	yStep *= (isRayFacingUp(rayAngle) && yStep > 0) ? -1 : 1;
	yStep *= (isRayFacingDown(rayAngle) && yStep < 0) ? -1 : 1;

	float nextVertTouchX = xIntercept;
	float nextVertTouchY = yIntercept;
	while (isInsideMap(nextVertTouchX, nextVertTouchY)) {
		float xToCheck = nextVertTouchX + (isRayFacingLeft(rayAngle) ? -1 : 0);
		float yToCheck = nextVertTouchY;

		vertHitType = hasWallAt(xToCheck, yToCheck);
		if (vertHitType > 0) {
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			foundVertWallHit = true;
			break;
		}
		else {
			nextVertTouchX += xStep;
			nextVertTouchY += yStep;
		}
	}

	//calculate vertical and horizontal distances & pick the shortest one.
	float horzHitDistance = foundHorzWallHit
		? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
		: INT_MAX;
	float vertHitDistance = foundVertWallHit
		? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
		: INT_MAX;

	if (vertHitDistance < horzHitDistance) {
		rays[stripId].distance = vertHitDistance;
		rays[stripId].wallHitType = vertHitType;
		rays[stripId].wallHitX = vertWallHitX;
		rays[stripId].wallHitY = vertWallHitY;
		rays[stripId].wasHitVertical = true;
		rays[stripId].rayAngle = rayAngle;
	}
	else {
		rays[stripId].distance = horzHitDistance;
		rays[stripId].wallHitType = horzHitType;
		rays[stripId].wallHitX = horzWallHitX;
		rays[stripId].wallHitY = horzWallHitY;
		rays[stripId].wasHitVertical = false;
		rays[stripId].rayAngle = rayAngle;
	}
}

void castAllRays() {
	float projectionPlaneDist = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
	for (int stripId = 0; stripId < NUM_RAYS; stripId++) {

		float rayAngle = player.rotationAngle + atan((stripId - NUM_RAYS / 2) / projectionPlaneDist);
		castRay(rayAngle, stripId);
	}
}

void renderRays() {
	for (int i = 0; i < NUM_RAYS; i++) {
		drawLine(
			MINIMAP_SCALE_FACTOR * player.x,
			MINIMAP_SCALE_FACTOR * player.y,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitY,
			RED
		);
	}
}