#include "display.h"
#include "mmu.h"
#include "gpu.h"

#include <stdio.h>

COLOUR display_framebuffer[160*144];

const COLOUR display_palette[4] = {
    { 255, 255, 255 },
    { 192, 192, 192 },
    { 96, 96, 96 },
    { 0, 0, 0 },
};

void display_render_scanline() {
	
	int mapOffset = (gpu.control & GPU_CONTROL_TILEMAP) ? 0x1c00 : 0x1800;
	mapOffset += (((gpu.scanline + gpu.scroll_y) & 255) >> 3) << 5;
	
	int lineOffset = (gpu.scroll_x >> 3);
	
	int x = gpu.scroll_x & 7;
	int y = (gpu.scanline + gpu.scroll_y) & 7;
	
	int pixelOffset;
    pixelOffset = gpu.scanline * 160;


	unsigned short tile = (unsigned short)mmu.vram[mapOffset + lineOffset];
	
	unsigned char scanlineRow[160];
	
	// if bg enabled
	int i;
	for(i = 0; i < 160; i++) {
		unsigned char colour = gpu_tiles[tile][y][x];
		
		scanlineRow[i] = colour;
		
		
        display_framebuffer[pixelOffset].r = gpu_background_palette[colour].r;
        display_framebuffer[pixelOffset].g = gpu_background_palette[colour].g;
        display_framebuffer[pixelOffset].b = gpu_background_palette[colour].b;
		
		x++;
		if(x == 8) {
			x = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = mmu.vram[mapOffset + lineOffset];
		}
	}
	
	// if sprites enabled
	for(i = 0; i < 40; i++) {
		struct sprite sprite = ((struct sprite *)mmu.oam)[i];
		
		int sx = sprite.x - 8;
		int sy = sprite.y - 16;
		
		if(sy <= gpu.scanline && (sy + 8) > gpu.scanline) {
			COLOUR *pal = gpu_sprite_palette[sprite.palette];
			
			int pixelOffset;
			pixelOffset = gpu.scanline * 160 + sx;
			
			unsigned char tileRow;
			if(sprite.vFlip) tileRow = 7 - (gpu.scanline - sy);
			else tileRow = gpu.scanline - sy;
			
			int x;
			for(x = 0; x < 8; x++) {
				if(sx + x >= 0 && sx + x < 160 && (~sprite.priority || !scanlineRow[sx + x])) {
					unsigned char colour;
					
					if(sprite.hFlip) colour = gpu_tiles[sprite.tile][tileRow][7 - x];
					else colour = gpu_tiles[sprite.tile][tileRow][x];
					
					if(colour) {
                        display_framebuffer[pixelOffset].r = pal[colour].r;
                        display_framebuffer[pixelOffset].g = pal[colour].g;
                        display_framebuffer[pixelOffset].b = pal[colour].b;

						pixelOffset++;
					}
				}
			}
		}
	}
}

void display_draw_framebuffer() {
    // to implement
	printf("test draw framebuffer\n");
}
