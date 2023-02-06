#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4

unsigned short cpu_get_combined_registers(unsigned char high, unsigned char low);
unsigned char cpu_compile_flag_register();
void cpu_set_combined_registers(unsigned char high, unsigned char low);
void cpu_emulate();
void cpu_reset();
void cpu_unimplemented_instruction();

struct cpu {
    bool stopped;
    float emulation_speed;
    int ticks;
    unsigned short operand;
    bool flag_zero;
    bool flag_subtract;
    bool flag_half_carry;
    bool flag_carry;
} extern cpu;

struct cpu_instruction {
    char mnemonic[20];
    int length;
    int cycles;
    void *function;
};
