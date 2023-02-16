#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "gpu.h"
#include "display.h"
#include "cpu.h"
#include "mmu.h"
#include "interrupts.h"

#define GAMEBOY_WIDTH 160
#define GAMEBOY_HEIGHT 144

SDL_Window* gpu_window = NULL;
SDL_Surface* gpu_surface = NULL;
SDL_Texture* gpu_texture = NULL;
SDL_Renderer* gpu_renderer = NULL;


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
		//Get window surface
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		SDL_CreateWindowAndRenderer(160, 144, 0, &gpu_window, &gpu_renderer);
		SDL_SetWindowSize(gpu_window, GAMEBOY_WIDTH * 3, GAMEBOY_WIDTH * 3);
		SDL_SetWindowResizable(gpu_window, SDL_TRUE);
		gpu_texture = SDL_CreateTexture(gpu_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 144);

    }

    return success;
}

void gpu_draw_framebuffer() {
    SDL_UpdateTexture(gpu_texture, NULL, display_framebuffer, GAMEBOY_WIDTH * sizeof(unsigned char) * 3);
    SDL_RenderClear(gpu_renderer);
    SDL_RenderCopy(gpu_renderer, gpu_texture, NULL, NULL);
    SDL_RenderPresent(gpu_renderer);
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
					if(interrupt.enable & INTERRUPTS_VBLANK) {
                        interrupt.flags |= INTERRUPTS_VBLANK;
                    }
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
				display_render_scanline();
				gpu.tick -= 172;
			}
			
			break;
	}
}

void gpu_hblank() {
	gpu.scanline++;
}

void gpu_render_tiles() {
	unsigned short tile_data = 0; 
	unsigned short bg_memory = 0;
	bool unsig = true;

	unsigned char scroll_y = mmu_read_byte(0xFF42);
	unsigned char scroll_x = mmu_read_byte(0xFF43);
	unsigned char window_y = mmu_read_byte(0xFF4A);
	unsigned char window_x = mmu_read_byte(0xFF4B) - 7;

	bool using_window = false;

	// is window enabled?
	if (GET_BIT(gpu.control, 5)) {
		// is the scanline within the window y pos?
		if (window_y <= gpu.scanline) {
			using_window = true;
		}
	}

	if (GET_BIT(gpu.control, 4)) {
		tile_data = 0x8000;
	}
	else {
		// using signed bytes as identifiers
		tile_data = 0x8800;
		unsig = false;
	}

	// Which background memory?
	if (using_window == false) {
		if(GET_BIT(gpu.control, 3)) {
			bg_memory = 0x9C00;
		}
		else {
			bg_memory = 0x9800;
		}
	}
	else {
		// which window memory?
		if (GET_BIT(gpu.control, 6)) {
			bg_memory = 0x9C00;
		}
		else {
			bg_memory = 0x9800;
		}
	}

	unsigned char y_pos = 0;

	// use y_pos to calculate which of 32 verticle tiles scanline is drawing
	if (!using_window) {
		y_pos = scroll_y + gpu.scanline;
	}
	else {
		y_pos = gpu.scanline - window_y;
	}

	// which of the 8 vertical pixels of the current tile is scanline on
	unsigned short tile_row = (((unsigned char)(y_pos / 8)) * 32);

	int pixel_offset = gpu.scanline * 160;

	// begin drawing the 160 horizontal pixels
	for (int pixel = 0; pixel < 160; pixel++) {
		unsigned char x_pos = pixel + scroll_x;

		// translate xpos to window space
		if (using_window) {
			if (pixel >= window_x) {
				x_pos = pixel - window_x;
			}
		}

		// which of the 32 horizontal tiles does x_pos fall in?
		unsigned short tile_col = (x_pos / 8);
		short tile_number;

		// get the tile ID number, it can be signed or unsigned
		unsigned short tile_address = bg_memory + tile_row + tile_col;
		
		if(unsig) {
			tile_number = mmu_read_byte(tile_address);
		}
		else {
			tile_number = (char)mmu_read_byte(tile_address);
		}

		// find where this tile identifier actually is in memory
		unsigned short tile_location = tile_data;

		if(unsig) {
			tile_location += (tile_number * 16);
		}
		else {
			tile_location += ((tile_number + 128) * 16);
		}

		// find the correct vertical line of the tile to get tile data
		unsigned char line = y_pos % 8;
		line *= 2; // each line takes up 2 bytes
		unsigned char data1 = mmu_read_byte(tile_location + line);
		unsigned char data2 = mmu_read_byte(tile_location + line + 1);

		int colour_bit = x_pos % 8;
		colour_bit -= 7;
		colour_bit *= -1;

		unsigned char colour_num1 = GET_BIT(data2, colour_bit);
		unsigned char colour_num2 = colour_num2 = GET_BIT(data1, colour_bit);

		int colour_id = gpu_get_colour_id(colour_num1, colour_num2);
		COLOUR pixel_colour = gpu_get_colour(0xFF47, colour_id);

		display_framebuffer[pixel_offset].r = pixel_colour.r;
		display_framebuffer[pixel_offset].g = pixel_colour.g;
		display_framebuffer[pixel_offset].b = pixel_colour.b;
		pixel_offset++;
	}
}

void gpu_render_sprites() {
	bool use_8x16 = false;

	if (GET_BIT(gpu.control, 2)) {
		use_8x16 = true;
	}

	for (int spritenum = 0; spritenum < 40; spritenum++) {
		unsigned char index = spritenum * 4;
		unsigned char x_pos = mmu_read_byte(0xFE00 + index + 1) - 8;
		unsigned char y_pos = mmu_read_byte(0xFE00 + index) - 16;
		unsigned char tile_location = mmu_read_byte(0xFE00 + index + 2);
		unsigned char attributes = mmu_read_byte(0xFe00 + index + 3);
		
		bool x_flip = CHECK_BIT(attributes, 6);
		bool y_flip = CHECK_BIT(attributes, 5);

		int y_size = 8;
		
		if (use_8x16) {
			y_size = 16;
		}

		// does sprite intercept with scanline?
		if ((gpu.scanline >= y_pos) && (gpu.scanline < (y_pos + y_size))) {
			int line = gpu.scanline - y_pos;

			// read sprite in backwards in y axis
			if (y_flip) {
				line -= y_size;
				line *= -1;
			}

			line *= 2;
			unsigned short tile_address = (0x8000 + (tile_location * 16)) + line;

			unsigned char data1 = mmu_read_byte(tile_address);
			unsigned char data2 = mmu_read_byte(tile_address + 1);

			int pixel_offset = gpu.scanline * 160 + x_pos;

			// read from right to left as pixel 0 is bit 7 in colour etc
			for (int tile_pixel = 7; tile_pixel >= 0; tile_pixel--) {
				int colour_bit = tile_pixel;
				// read backwards for x axis
				if (x_flip) {
					colour_bit -= 7;
					colour_bit *= -1;
				}

				
				unsigned char colour_num1 = GET_BIT(data2, colour_bit);
				unsigned char colour_num2 = colour_num2 = GET_BIT(data1, colour_bit);

				unsigned char colour_address = GET_BIT(attributes, 4)?0xFF49:0xFF48;
				int colour_id = gpu_get_colour_id(colour_num1, colour_num2);
				COLOUR pixel_colour = gpu_get_colour(colour_address, colour_id);
		
				if (colour_id == 0) continue;

				int x_pixel = 0 - tile_pixel;
				x_pixel += 7;

				int pixel = x_pos + x_pixel;

				display_framebuffer[pixel_offset + x_pixel].r = pixel_colour.r;
				display_framebuffer[pixel_offset  + x_pixel].g = pixel_colour.g;
				display_framebuffer[pixel_offset + x_pixel].b = pixel_colour.b;
			}
		}


	}

}

int gpu_get_colour_id(unsigned char code1, unsigned char code2) {
	if (code1 == 0 && code2 == 0) return 0;
	if (code1 == 0 && code2 == 1) return 1;
	if (code1 == 1 && code2 == 0) return 2;
	if (code1 == 1 && code2 == 1) return 3;

}

COLOUR gpu_get_colour(unsigned short address, int colour_id) {
	COLOUR palette_colours[4];
	unsigned char value = mmu_read_byte(address);

	for (int i = 0; i < 4; i++) {
		palette_colours[i] = display_palette[(value >> (i * 2)) & 3];
	}

	return palette_colours[colour_id];
}