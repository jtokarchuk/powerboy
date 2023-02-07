#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FLAGS_ZERO (1 << 7)
#define FLAGS_NEGATIVE (1 << 6)
#define FLAGS_HALFCARRY (1 << 5)
#define FLAGS_CARRY (1 << 4)

#define FLAGS_ISZERO (registers.f & FLAGS_ZERO)
#define FLAGS_ISNEGATIVE (registers.f & FLAGS_NEGATIVE)
#define FLAGS_ISCARRY (registers.f & FLAGS_CARRY)
#define FLAGS_ISHALFCARRY (registers.f & FLAGS_HALFCARRY)

#define FLAGS_ISSET(x) (registers.f & (x))
#define FLAGS_SET(x) (registers.f |= (x))
#define FLAGS_CLEAR(x) (registers.f &= ~(x))

#define CPU_CLOCKSPEED 4194304;

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
    unsigned short last_instruction;
} extern cpu;

struct cpu_instruction {
    char mnemonic[20];
    int length;
    int cycles;
    void *function;
};

// 0x00
void cpu_nop();
//0x01
void cpu_ld_bc_nn(unsigned short operand);
//0xc3
void jp_nn(unsigned short operand);
//0xcf
void rst_1();