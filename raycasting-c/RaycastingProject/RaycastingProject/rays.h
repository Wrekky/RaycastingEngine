#ifndef RAY_H
#define RAY_H

#include <stdbool.h>
#include <limits.h>
#include "utils.h"
#include "color.h"
#include "defs.h"
#include "player.h"


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

void castRay(float rayAngle, int stripId);

void castAllRays();

void renderRays();
#endif