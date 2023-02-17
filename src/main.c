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
#include "timer.h"

bool application_closing = false;

int main(int argc, char *argv[]) {
    printf("PowerEmu Initializing...\n");
    unsigned int a = SDL_GetTicks();
    unsigned int b = SDL_GetTicks();
    double delta = 0;

    char *filename;
	int i;

	for(i = 1; i < argc; i++) {
		filename = argv[i];
	}
    
    cpu_reset();
    timer_reset();

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
        a = SDL_GetTicks();

        
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
                            cpu.halted = false;
                            break;
                        case SDLK_DOWN:
                            keys.down = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_LEFT:
                            keys.left = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_RIGHT:
                            keys.right = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_x:
                            keys.a = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_z:
                            keys.b = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_RETURN:
                            keys.start = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_SPACE:
                            keys.select = 0;
                            cpu.halted = false;
                            break;
                        case SDLK_q:
                            cpu.debug_key = true;
                            break;
                    }
                    break;

                case SDL_KEYUP:  // Handle a KEYDOWN event 
                    switch( gpu_sdl_event.key.keysym.sym ) {
                        case SDLK_ESCAPE:
                            application_closing = true;
                            break;
                        case SDLK_UP:
                            keys.up = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_DOWN:
                            keys.down = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_LEFT:
                            keys.left = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_RIGHT:
                            keys.right = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_x:
                            keys.a = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_z:
                            keys.b = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_RETURN:
                            keys.start = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_SPACE:
                            keys.select = 1;
                            cpu.halted = false;
                            break;
                        case SDLK_q:
                            cpu.debug_key = false;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    break;
                default: 
                    break;
            }
           
        } 
        
        delta = a - b;
        if (delta > 1000/60.0) {  
            int cpu_cycles = 0;
            int cpu_max = 69905;
            
            while (cpu_cycles <= cpu_max) {
                cpu_emulate();
                gpu_emulate();
                timer_emulate(cpu.ticks);
                interrupts_emulate();
                cpu_cycles += cpu.ticks;  
            }
            cpu_cycles = 0;
            b = a;   
        } 
    }
    

    gpu_exit();
    printf("PowerEmu Shutting Down\n");
    return 0;
}

