#include "rays.h"

ray_t rays[NUM_RAYS];

float normalizeAngle(float angle) {
	angle = remainder(angle, TWO_PI);
	if (angle < 0) {
		angle = TWO_PI + angle;
	}
	return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int stripId) {
	rayAngle = normalizeAngle(rayAngle);

	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;

	int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
	int isRayFacingLeft = !isRayFacingRight;
	float xIntercept, yIntercept;
	float xStep, yStep;

	//Horizontal Ray-grid intersection code
	bool foundHorzWallHit = false;
	float horzWallHitX = 0;
	float horzWallHitY = 0;
	int horzHitType = 0;

	yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yIntercept += isRayFacingDown ? TILE_SIZE : 0;

	xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

	yStep = TILE_SIZE;
	yStep *= isRayFacingUp ? -1 : 1;

	xStep = TILE_SIZE / tan(rayAngle);
	xStep *= (isRayFacingLeft && xStep > 0) ? -1 : 1;
	xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;
	float nextHorzTouchX = xIntercept;
	float nextHorzTouchY = yIntercept;

	while (isInsideMap(nextHorzTouchX, nextHorzTouchY)) {
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

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
	xIntercept += isRayFacingRight ? TILE_SIZE : 0;

	yIntercept = player.y + (xIntercept - player.x) * tan(rayAngle);

	xStep = TILE_SIZE;
	xStep *= isRayFacingLeft ? -1 : 1;

	yStep = TILE_SIZE * tan(rayAngle);
	yStep *= (isRayFacingUp && yStep > 0) ? -1 : 1;
	yStep *= (isRayFacingDown && yStep < 0) ? -1 : 1;

	float nextVertTouchX = xIntercept;
	float nextVertTouchY = yIntercept;
	while (isInsideMap(nextVertTouchX, nextVertTouchY)) {
		float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
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
	/*
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int i = 0; i < NUM_RAYS; i++) {
		SDL_RenderLine(
			renderer,
			MINIMAP_SCALE_FACTOR * player.x,
			MINIMAP_SCALE_FACTOR * player.y,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitY
		);
	}
	*/
}