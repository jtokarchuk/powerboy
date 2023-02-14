#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>

#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "interrupts.h"
#include "rom.h"
#include "keys.h"
#include "registers.h"

bool application_closing = false;

int main(int argc, char *argv[]) {
    printf("PowerEmu Initializing...\n");

    char *filename;

    printf("argc = %d\n", argc);
	int i;
	for(i = 1; i < argc; i++) {
		filename = argv[i];
	}
    
    cpu_reset();

    if (!gpu_init()) {
        printf("Failed to initialize GPU, exitting\n");
        return 1;
    }

    // load a ROM
    if(filename == NULL) {
		printf("No ROM input\n");
		
		gpu_exit();
		return 1;
	}

    if (!rom_load(filename))
    {
        printf("ROM File load failed!\n");
        gpu_exit();
        return 1;
    }

    printf("ROM Loaded, starting emulation\n");

    while( !application_closing ) {

        while (SDL_PollEvent(&gpu_sdl_event)) {
			switch (gpu_sdl_event.type) {
				case SDL_QUIT:
                    printf("Got close event\n");
					application_closing = true;
                    break;
                case SDL_KEYDOWN:
                    switch( gpu_sdl_event.key.keysym.sym ) {
                        case SDLK_ESCAPE:
                            application_closing = true;
                            break;
                        case SDLK_UP:
                            keys.up = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_DOWN:
                            keys.down = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_LEFT:
                            keys.left = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_RIGHT:
                            keys.right = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_x:
                            keys.a = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_z:
                            keys.b = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_RETURN:
                            keys.start = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_SPACE:
                            keys.select = 0;
                            cpu.stopped = false;
                            break;
                        case SDLK_q:
                            cpu.debug_key = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:  /* Handle a KEYDOWN event */
                    switch( gpu_sdl_event.key.keysym.sym ) {
                        case SDLK_ESCAPE:
                            application_closing = true;
                            break;
                        case SDLK_UP:
                            keys.up = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_DOWN:
                            keys.down = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_LEFT:
                            keys.left = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_RIGHT:
                            keys.right = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_x:
                            keys.a = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_z:
                            keys.b = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_RETURN:
                            keys.start = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_SPACE:
                            keys.select = 1;
                            cpu.stopped = false;
                            break;
                        case SDLK_q:
                            cpu.debug_key = false;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    break;
                default: /* Report an unhandled event */
                    // printf("Unknown Event: %d\n", gpu_sdl_event.type);
                    break;
            }
        }

        if (!cpu.stopped) {
                fprintf(pFile, "A:%02hX F:%02hX B:%02hX C:%02hX D:%02hX E:%02hX H:%02hX L:%02hX SP:%04hX PC:%04hX PCMEM:%02hX,%02hX,%02hX,%02hX\n", \
        registers.a, registers.f, registers.b, registers.c, registers.d, registers.e, registers.h, registers.l, registers.sp, \
        registers.pc, mmu_read_byte(registers.pc), mmu_read_byte(registers.pc + 1), mmu_read_byte(registers.pc + 2), mmu_read_byte(registers.pc + 3));
            cpu_emulate();
            gpu_emulate();
            interrupts_emulate();
        }
    

    }
    
    gpu_exit();
    printf("PowerEmu Shutting Down\n");
    fclose(pFile);
    return 0;
}

