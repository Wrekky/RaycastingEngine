#ifndef RAY_H
#define RAY_H

#include <stdbool.h>
#include "defs.h"
#include "player.h"
#include <limits.h>

typedef struct {
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;

	int wallHitType;

	bool wasHitVertical;
} ray_t;

extern ray_t rays[NUM_RAYS];

bool isRayFacingUp(float angle);
bool isRayFacingDown(float angle);
bool isRayFacingLeft(float angle);
bool isRayFacingRight(float angle);

void normalizeAngle(float* angle);

float distanceBetweenPoints(float x1, float y1, float x2, float y2);

void castRay(float rayAngle, int stripId);

void castAllRays();

void renderRays();
#endif