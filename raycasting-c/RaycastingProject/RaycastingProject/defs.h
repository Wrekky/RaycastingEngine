
#include <stdint.h>

#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64

#define MINIMAP_SCALE_FACTOR 0.3
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define FOV_ANGLE (75 * (PI / 180))

#define NUM_RAYS WINDOW_WIDTH

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

typedef uint32_t color_t;