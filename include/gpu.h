#pragma once
#include <SDL.h>

bool gpu_init();
bool gpu_exit();

//The window we'll be rendering to
SDL_Window* gpu_window = NULL;
    
//The surface contained by the window
SDL_Surface* gpu_surface = NULL;

//The image we will load and show on the screen
SDL_Surface* gpu_display = NULL;