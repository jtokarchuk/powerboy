#include <stdio.h>
#include <stdbool.h>

bool application_closing = false;


int main(int argc, char *argv[]) {
    printf("PowerEmu Initializing...\n");

    while( !application_closing ) {

    }
    
    printf("PowerEmu Shutting Down");

    return 0;
} 
