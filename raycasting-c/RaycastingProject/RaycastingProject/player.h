#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"
#include "map.h"
#include <stdint.h>
typedef struct {
	float x;
	float y;
	float width;
	float height;
	int turnDirection; //-1 left, +1 right
	int walkDirection; //-1 backwards, += forwards
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player_t;

extern player_t player;

void movePlayer(float deltaTime);

void renderPlayer();
#endif