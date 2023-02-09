#pragma once
#include "platform.h"

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

typedef struct rgb COLOUR;

extern COLOUR display_framebuffer[160 * 144];
extern const COLOUR display_palette[4];

extern void display_render_scanline();