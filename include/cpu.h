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

void cpu_nop();
void cpu_ld_bc_nn(unsigned short operand);
void jp_nn(unsigned short operand);

void rst_1();
void rst_7();

void inc_a();
void inc_b();
void inc_c();
void inc_d();
void inc_e();
void inc_h();
void inc_l();
void inc_bc();
void inc_de();
void inc_hl();
void inc_sp();

void ld_bcp_a();

void call_nn(unsigned short operand);

void sub_n(unsigned char operand);