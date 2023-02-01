#ifndef _gameboy_z80
#define _gameboy_z80

#include <stdint.h>

struct cpu {
    struct registers {
        uint8_t A; 
        uint8_t F; 
        uint8_t B;
        uint8_t C;
        uint8_t D;
        uint8_t E;
        uint8_t H;
        uint8_t L;
    };
    uint16_t PC;
    uint16_t SP;
    uint8_t flags;
    unsigned short opcode;
};

uint16_t cpu_combine_registers(uint8_t high, uint8_t low);

#endif