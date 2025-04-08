#include "textures.h"
#include <stdio.h>
texture_t textures[NUM_TEXTURES];

static const char* textureFileNames[NUM_TEXTURES] = {
    "./images/redbrick.png",
    "./images/purplestone.png",
    "./images/mossystone.png",
    "./images/graystone.png",
    "./images/colorstone.png",
    "./images/bluestone.png",
    "./images/wood.png",
    "./images/eagle.png",
    "./images/pikuma.png",
};

void loadTextures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_t* upng;

        upng = upng_new_from_file(textureFileNames[i]);
        if (upng != NULL) {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK) {
                textures[i].upngTexture = upng;
                textures[i].width = upng_get_width(upng);
                textures[i].height = upng_get_height(upng);
                textures[i].texture_buffer = (uint32_t*)upng_get_buffer(upng);
            }
        }
    }
}

void freeTextures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_free(textures[i].upngTexture);
    }
}