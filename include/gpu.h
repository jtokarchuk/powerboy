#pragma once
#include "platform.h"

#define GPU_CONTROL_BGENABLE (1 << 0)
#define GPU_CONTROL_SPRITEENABLE (1 << 1)
#define GPU_CONTROL_SPRITEVDOUBLE (1 << 2)
#define GPU_CONTROL_TILEMAP (1 << 3)
#define GPU_CONTROL_TILESET (1 << 4)
#define GPU_CONTROL_WINDOWENABLE (1 << 5)
#define GPU_CONTROL_WINDOWTILEMAP (1 << 6)
#define GPU_CONTROL_DISPLAYENABLE (1 << 7)

#include <SDL.h>
#include <stdbool.h>
#include <assert.h>

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

extern SDL_Window* gpu_window;
extern SDL_Surface* gpu_surface;
extern SDL_Renderer* gpu_renderer;
extern SDL_Texture* gpu_texture;

typedef struct rgb COLOUR;
extern int gpu_scale_factor;

extern COLOUR gpu_framebuffer[160 * 144]; 
extern const COLOUR gpu_palette[4];

extern COLOUR gpu_background_palette[4];
extern COLOUR gpu_sprite_palette[2][4];

bool gpu_init();
void gpu_exit();
void gpu_emulate();
void gpu_draw_framebuffer();
extern void gpu_render_scanline();

extern SDL_Event gpu_sdl_event;

struct gpu {
	unsigned char control;
	unsigned char scroll_x;
	unsigned char scroll_y;
	unsigned char window_x;
	unsigned char window_y;
	unsigned char scanline;
	unsigned long tick;
} extern gpu;

struct sprite {
	#ifdef LITTLE_E
		unsigned char y;
		unsigned char x;
		unsigned char tile;

		unsigned char priority : 1;
		unsigned char vFlip : 1;
		unsigned char hFlip : 1;
		unsigned char palette : 1;

	#else
		unsigned char y;
		unsigned char x;
		unsigned char tile;

		//unsigned char dummy : 4;
		unsigned char palette : 1;
		unsigned char hFlip : 1;
		unsigned char vFlip : 1;
		unsigned char priority : 1;
	#endif
};

void gpu_hblank();
void gpu_render_tiles();
void gpu_render_sprites();

int gpu_get_colour_id(unsigned char code1, unsigned char code2);
void gpu_update_tile(unsigned short address, unsigned char value);