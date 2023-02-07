#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#include "cpu.h"
#include "gpu.h"
#include "interrupts.h"

bool application_closing = false;


int main(int argc, char *argv[]) {
    printf("PowerEmu Initializing...\n");
    
    cpu_reset();

    if (!gpu_init()) {
        printf("Failed to initialize GPU, exitting\n");
        return 1;
    }

    // load a ROM

    while( !application_closing ) {
        cpu_emulate();
        gpu_emulate()
        // sound_emulate() ?
        interrupts_emulate()
    }
    
    printf("PowerEmu Shutting Down");

    gpu_exit();

    return 0;
}
