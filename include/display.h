#pragma once

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

typedef struct rgb COLOUR;

extern COLOUR display_framebuffer[160 * 144];
void display_draw_framebuffer();
extern const COLOUR palette[4];