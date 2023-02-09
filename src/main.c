#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>

#include "cpu.h"
#include "gpu.h"
#include "interrupts.h"
#include "rom.h"
#include "keys.h"

bool application_closing = false;

int main(int argc, char *argv[]) {
    printf("PowerEmu Initializing...\n");

    unsigned int start = SDL_GetTicks();
    unsigned int end = SDL_GetTicks();
    double delta = 0;

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
                case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
                    printf("Key down %d\n", gpu_sdl_event.key.keysym.sym);
                    if(gpu_sdl_event.key.keysym.sym == SDLK_ESCAPE){
                        application_closing = true;
                        break;
                    }
                    break;
                case SDL_KEYUP:
                    printf("Key up: %d\n", gpu_sdl_event.key.keysym.sym);
                    break;
                case SDL_MOUSEMOTION:
                    break;
                default: /* Report an unhandled event */
                    // printf("Unknown Event: %d\n", gpu_sdl_event.type);
                    break;
            }
        }

        if (!cpu.stopped) {
            cpu_emulate();
            gpu_emulate();
            interrupts_emulate();
            delta = 0;
        }
    

    }
    
    gpu_exit();
    printf("PowerEmu Shutting Down\n");
    return 0;
}

