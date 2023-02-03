#pragma once

#include <stdint.h>
#include <stdbool.h>

struct registers {
    unsigned char a; 
    unsigned char f; 
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char e;
    unsigned char h;
    unsigned char l;
    unsigned short pc;
    unsigned short sp;
} extern registers;