#pragma once

#include <stdint.h>
#include <stdbool.h>

const uint8_t ZERO_FLAG_BYTE_POSITION  = 7;
const uint8_t SUBTRACT_FLAG_BYTE_POSITION  = 6;
const uint8_t HALF_CARRY_FLAG_BYTE_POSITION  = 5;
const uint8_t CARRY_FLAG_BYTE_POSITION  = 4;

uint16_t cpu_get_combined_registers(uint8_t high, uint8_t low);
void cpu_set_combined_registers(uint8_t high, uint8_t low);
uint8_t cpu_compile_flag_register();
bool cpu_unimplemented_instruction(bool CB, uint8_t instruction);
void cpu_emulate(int cycles);


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

    struct flags {
        bool zero;
        bool subtract;
        bool half_carry;
        bool carry;
    };

    unsigned short opcode;

};

struct cpu_instruction {
    char mnemonic[20];
    int length;
    int cycles;
};
