#include "display.h"
#include "mmu.h"
#include "gpu.h"
#include "cpu.h"

#include <stdio.h>

COLOUR display_framebuffer[160 * 144];

const COLOUR display_palette[4] = {
    { 255, 255, 255 },
    { 192, 192, 192 },
    { 96, 96, 96 },
    { 0, 0, 0 },
};

void display_render_scanline() {
	unsigned char control = mmu_read_byte(0xFF40);

	if (CHECK_BIT(control, 0)) {
		gpu_render_tiles();
	}
	if (CHECK_BIT(control, 1)) {
		gpu_render_sprites();
	}
}


