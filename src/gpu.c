#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "gpu.h"
#include "display.h"
#include "cpu.h"
#include "mmu.h"
#include "interrupts.h"

SDL_Window* gpu_window = NULL;
SDL_Surface* gpu_surface = NULL;
SDL_Surface* gpu_display = NULL;

COLOUR gpu_background_palette[4];
COLOUR gpu_sprite_palette[2][4];
unsigned char gpu_tiles[384][8][8];

SDL_Event gpu_sdl_event;


struct gpu gpu;

bool gpu_init() {
    
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gpu_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, SDL_WINDOW_SHOWN );
        if( gpu_window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            gpu_surface = SDL_GetWindowSurface( gpu_window );
        }
    }

    return success;
}

void gpu_exit() {
    //Deallocate surface

    //Destroy window
    SDL_DestroyWindow( gpu_window );
    gpu_window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void gpu_emulate(void) {
	enum gpuMode {
		GPU_MODE_HBLANK = 0,
		GPU_MODE_VBLANK = 1,
		GPU_MODE_OAM = 2,
		GPU_MODE_VRAM = 3,
	} static gpuMode = GPU_MODE_HBLANK;
	
	static int lastTicks = 0;
	
	gpu.tick += cpu.ticks - lastTicks;
	
	lastTicks = cpu.ticks;
	
	switch(gpuMode) {
		case GPU_MODE_HBLANK:
			if(gpu.tick >= 204) {
				gpu_hblank();
				
				if(gpu.scanline == 143) {
					if(interrupt.enable & INTERRUPTS_VBLANK) interrupt.flags |= INTERRUPTS_VBLANK;
					
					gpuMode = GPU_MODE_VBLANK;
				}
				
				else gpuMode = GPU_MODE_OAM;
				
				gpu.tick -= 204;
			}
			
			break;
		
		case GPU_MODE_VBLANK:
			if(gpu.tick >= 456) {
				gpu.scanline++;
				
				if(gpu.scanline > 153) {
					gpu.scanline = 0;
					gpuMode = GPU_MODE_OAM;
				}
				
				gpu.tick -= 456;
			}
			
			break;
		
		case GPU_MODE_OAM:
			if(gpu.tick >= 80) {
				gpuMode = GPU_MODE_VRAM;
				
				gpu.tick -= 80;
			}
			
			break;
		
		case GPU_MODE_VRAM:
			if(gpu.tick >= 172) {
				gpuMode = GPU_MODE_HBLANK;
				printf("gpu tick hblank");
				display_render_scanline();
				gpu.tick -= 172;
			}
			
			break;
	}
}

void gpu_hblank(void) {
	gpu.scanline++;
}

void gpu_update_tile(unsigned short address, unsigned char value) {
	address &= 0x1ffe;
	
	unsigned short tile = (address >> 4) & 511;
	unsigned short y = (address >> 1) & 7;
	
	unsigned char x, bitIndex;
	for(x = 0; x < 8; x++) {
		bitIndex = 1 << (7 - x);
		
		//((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
		gpu_tiles[tile][y][x] = ((mmu.vram[address] & bitIndex) ? 1 : 0) + ((mmu.vram[address + 1] & bitIndex) ? 2 : 0);
	}
}