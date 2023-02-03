#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"

bool application_closing = false;


int main(int argc, char *argv[]) {
    printf("PowerEmu Initializing...\n");
    cpu_reset();
    // load GB BIOS
    // load a ROM

    while( !application_closing ) {
        cpu_emulate();
        // gpu_emulate()
        // sound_emulate() ?
        // interrupt_emulate()
    }
    
    printf("PowerEmu Shutting Down");

    return 0;
} 
