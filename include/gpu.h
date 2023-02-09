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

#define GPU_MAX_FPS 60

#include <SDL.h>
#include <stdbool.h>
#include <assert.h>
#include "display.h"

bool gpu_init();
void gpu_exit();
void gpu_emulate();
void gpu_draw_framebuffer();

extern SDL_Event gpu_sdl_event;
extern int gpu_counted_frames;

//The window we'll be rendering to
extern SDL_Window* gpu_window;
    
//The surface contained by the window
extern SDL_Surface* gpu_surface;

extern SDL_Renderer* gpu_renderer;

//The image we will load and show on the screen
extern SDL_Texture* gpu_texture;

struct gpu {
	unsigned char control;
	unsigned char scroll_x;
	unsigned char scroll_y;
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

extern unsigned char gpu_tiles[384][8][8];

extern COLOUR gpu_background_palette[4];
extern COLOUR gpu_sprite_palette[2][4];


void gpu_hblank();

void gpu_update_tile(unsigned short address, unsigned char value);