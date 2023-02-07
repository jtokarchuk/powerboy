#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "gpu.h"
#include "display.h"
#include "registers.h"
#include "mmu.h"
#include "interrupts.h"

struct cpu cpu;

const struct cpu_instruction cpu_instructions[256] = {
    { "NOP", 0, 2, cpu_nop },                // 0x00
    { "LD BC, d16", 2, 6, cpu_ld_bc_nn },         
    { "LD (BC), A", 0, 4, ld_bcp_a },         
    { "INC BC", 0, 4, inc_bc },             
    { "INC B", 0, 2, inc_b },             
    { "DEC B", 0, 2, cpu_unimplemented_instruction },
    { "LD B, d8", 1, 4, cpu_unimplemented_instruction },
    { "RLCA", 0, 4, cpu_unimplemented_instruction },
    { "LD (a16), SP", 2, 10, cpu_unimplemented_instruction },
    { "ADD HL, BC", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (BC)", 0, 4, cpu_unimplemented_instruction },
    { "DEC BC", 0, 4, cpu_unimplemented_instruction },
    { "INC C", 0, 2, inc_c },
    { "DEC C", 0, 2, cpu_unimplemented_instruction },
    { "LD C, d8", 1, 2, cpu_unimplemented_instruction },
    { "RRCA", 0, 4, cpu_unimplemented_instruction },                         
    { "STOP", 1, 2, cpu_unimplemented_instruction },               // 0x10
    { "LD DE, d16", 2, 6, cpu_unimplemented_instruction },
    { "LD (DE), A", 0, 4, cpu_unimplemented_instruction },
    { "INC DE", 0, 4, inc_de },
    { "INC D", 0, 2, inc_d },
    { "DEC D", 0, 2, cpu_unimplemented_instruction },
    { "LD D, d8", 1, 4, cpu_unimplemented_instruction },
    { "RLA", 0, 4, cpu_unimplemented_instruction },
    { "JR s8", 1, 10, cpu_unimplemented_instruction },
    { "ADD HL, DE", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (DE)", 0, 4, cpu_unimplemented_instruction },
    { "DEC DE", 0, 4, cpu_unimplemented_instruction },
    { "INC E", 0, 2, inc_e },
    { "DEC E", 0, 2, cpu_unimplemented_instruction },
    { "LD E, d8", 1, 4, cpu_unimplemented_instruction },
    { "RRA", 0, 4, cpu_unimplemented_instruction },
    { "JR NZ, s8", 1, 2, cpu_unimplemented_instruction},          // 0x20
    { "LD HL, d16", 2, 6, cpu_unimplemented_instruction },
    { "LD (HL+), A", 0, 4, cpu_unimplemented_instruction },
    { "INC HL", 0, 4, inc_hl },
    { "INC H", 0, 2, inc_h },
    { "DEC H", 0, 2, cpu_unimplemented_instruction },
    { "LD H, d8", 1, 4, cpu_unimplemented_instruction },
    { "DAA", 0, 2, cpu_unimplemented_instruction },
    { "JR Z, s8", 1, 0, cpu_unimplemented_instruction},
    { "ADD HL, HL", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (HL+)", 0, 4, cpu_unimplemented_instruction },
    { "DEC HL", 0, 4, cpu_unimplemented_instruction },
    { "INC L", 0, 2, inc_l },
    { "DEC L", 0, 2, cpu_unimplemented_instruction },
    { "LD L, d8", 1, 4, cpu_unimplemented_instruction },
    { "CPL", 0, 2, cpu_unimplemented_instruction },
    { "JR NC, s8", 1, 4, cpu_unimplemented_instruction},          // 0x30
    { "LD SP, d16", 2, 6, cpu_unimplemented_instruction },
    { "LD (HL-), A", 0, 4, cpu_unimplemented_instruction },
    { "INC SP", 0, 4, inc_sp },
    { "INC (HL)", 0, 6, cpu_unimplemented_instruction },
    { "DEC (HL)", 0, 6, cpu_unimplemented_instruction},
    { "LD (HL), d8", 1, 6, cpu_unimplemented_instruction },
    { "SCF", 0, 2, cpu_unimplemented_instruction },
    { "JR C, s8", 1, 0, cpu_unimplemented_instruction },
    { "ADD HL, SP", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (HL-)", 0, 4, cpu_unimplemented_instruction },
    { "DEC SP", 0, 4, cpu_unimplemented_instruction },
    { "INC A", 0, 2, inc_a },
    { "DEC A", 0, 2, cpu_unimplemented_instruction },
    { "LD A, d8", 1, 4, cpu_unimplemented_instruction },
    { "CCF", 0, 2, cpu_unimplemented_instruction },
    { "LD B, B", 0, 2, cpu_unimplemented_instruction },            // 0x40
    { "LD B, C", 0, 2, cpu_unimplemented_instruction },
    { "LD B, D", 0, 2, cpu_unimplemented_instruction },
    { "LD B, E", 0, 2, cpu_unimplemented_instruction },
    { "LD B, H", 0, 2, cpu_unimplemented_instruction },
    { "LD B, L", 0, 2, cpu_unimplemented_instruction },
    { "LD B, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD B, A", 0, 2, cpu_unimplemented_instruction },
    { "LD C, B", 0, 2, cpu_unimplemented_instruction },
    { "LD C, C", 0, 2, cpu_unimplemented_instruction },
    { "LD C, D", 0, 2, cpu_unimplemented_instruction },
    { "LD C, E", 0, 2, cpu_unimplemented_instruction },
    { "LD C, H", 0, 2, cpu_unimplemented_instruction },
    { "LD C, L", 0, 2, cpu_unimplemented_instruction },
    { "LD C, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD C, A", 0, 2, cpu_unimplemented_instruction },
    { "LD D, B", 0, 2, cpu_unimplemented_instruction },             // 0x50
    { "LD D, C", 0, 2, cpu_unimplemented_instruction },
    { "LD D, D", 0, 2, cpu_unimplemented_instruction },
    { "LD D, E", 0, 2, cpu_unimplemented_instruction },
    { "LD D, H", 0, 2, cpu_unimplemented_instruction },
    { "LD D, L", 0, 2, cpu_unimplemented_instruction },
    { "LD D, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD D, A", 0, 2, cpu_unimplemented_instruction },
    { "LD E, B", 0, 2, cpu_unimplemented_instruction },
    { "LD E, C", 0, 2, cpu_unimplemented_instruction },
    { "LD E, D", 0, 2, cpu_unimplemented_instruction },
    { "LD E, E", 0, 2, cpu_unimplemented_instruction },
    { "LD E, H", 0, 2, cpu_unimplemented_instruction },
    { "LD E, L", 0, 2, cpu_unimplemented_instruction },
    { "LD E, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD E, A", 0, 2, cpu_unimplemented_instruction },
    { "LD H, B", 0, 2, cpu_unimplemented_instruction },              // 0x60
    { "LD H, C", 0, 2, cpu_unimplemented_instruction },
    { "LD H, D", 0, 2, cpu_unimplemented_instruction },
    { "LD H, E", 0, 2, cpu_unimplemented_instruction },
    { "LD H, H", 0, 2, cpu_unimplemented_instruction },
    { "LD H, L", 0, 2, cpu_unimplemented_instruction },
    { "LD H, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD H, A", 0, 2, cpu_unimplemented_instruction },
    { "LD L, B", 0, 2, cpu_unimplemented_instruction },
    { "LD L, C", 0, 2, cpu_unimplemented_instruction },
    { "LD L, D", 0, 2, cpu_unimplemented_instruction },
    { "LD L, E", 0, 2, cpu_unimplemented_instruction },
    { "LD L, H", 0, 2, cpu_unimplemented_instruction },
    { "LD L, L", 0, 2, cpu_unimplemented_instruction },
    { "LD L, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD L, A", 0, 2, cpu_unimplemented_instruction },
    { "LD (HL), B", 0, 4, cpu_unimplemented_instruction },            // 0x70
    { "LD (HL), C", 0, 4, cpu_unimplemented_instruction },
    { "LD (HL), D", 0, 4, cpu_unimplemented_instruction },
    { "LD (HL), E", 0, 4, cpu_unimplemented_instruction },
    { "LD (HL), H", 0, 4, cpu_unimplemented_instruction },
    { "LD (HL), L", 0, 4, cpu_unimplemented_instruction },
    { "HALT", 0, 2, cpu_unimplemented_instruction },
    { "LD (HL), A", 0, 4, cpu_unimplemented_instruction },
    { "LD A, B", 0, 2, cpu_unimplemented_instruction },
    { "LD A, C", 0, 2, cpu_unimplemented_instruction },
    { "LD A, D", 0, 2, cpu_unimplemented_instruction },
    { "LD A, E", 0, 2, cpu_unimplemented_instruction },
    { "LD A, H", 0, 2, cpu_unimplemented_instruction },
    { "LD A, L", 0, 2, cpu_unimplemented_instruction },
    { "LD A, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "LD A, A", 0, 2, cpu_unimplemented_instruction },
    { "ADD A, B", 0, 2, cpu_unimplemented_instruction },               // 0x80
    { "ADD A, C", 0, 2, cpu_unimplemented_instruction },
    { "ADD A, D", 0, 2, cpu_unimplemented_instruction },
    { "ADD A, E", 0, 2, cpu_unimplemented_instruction },
    { "ADD A, H", 0, 2, cpu_unimplemented_instruction },
    { "ADD A, L", 0, 2, cpu_unimplemented_instruction },
    { "ADD A, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "ADD A, A", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, B", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, C", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, D", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, E", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, H", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, L", 0, 2, cpu_unimplemented_instruction },
    { "ADC A, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "ADC A, A", 0, 2, cpu_unimplemented_instruction },
    { "SUB B", 0, 2, cpu_unimplemented_instruction },                  // 0x90
    { "SUB C", 0, 2, cpu_unimplemented_instruction },
    { "SUB D", 0, 2, cpu_unimplemented_instruction },
    { "SUB E", 0, 2, cpu_unimplemented_instruction },
    { "SUB H", 0, 2, cpu_unimplemented_instruction },
    { "SUB L", 0, 2, cpu_unimplemented_instruction },
    { "SUB (HL)", 0, 4, cpu_unimplemented_instruction },
    { "SUB A", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, B", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, C", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, D", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, E", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, H", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, L", 0, 2, cpu_unimplemented_instruction },
    { "SBC A, (HL)", 0, 4, cpu_unimplemented_instruction },
    { "SBC A, A", 0, 2, cpu_unimplemented_instruction },
    { "AND B", 0, 2, cpu_unimplemented_instruction },                   // 0xA0
    { "AND C", 0, 2, cpu_unimplemented_instruction },
    { "AND D", 0, 2, cpu_unimplemented_instruction },
    { "AND E", 0, 2, cpu_unimplemented_instruction },
    { "AND H", 0, 2, cpu_unimplemented_instruction },
    { "AND L", 0, 2, cpu_unimplemented_instruction },
    { "AND (HL)", 0, 4, cpu_unimplemented_instruction },
    { "AND A", 0, 2, cpu_unimplemented_instruction },
    { "XOR B", 0, 2, cpu_unimplemented_instruction },
    { "XOR C", 0, 2, cpu_unimplemented_instruction },
    { "XOR D", 0, 2, cpu_unimplemented_instruction },
    { "XOR E", 0, 2, cpu_unimplemented_instruction },
    { "XOR H", 0, 2, cpu_unimplemented_instruction },
    { "XOR L", 0, 2, cpu_unimplemented_instruction },
    { "XOR (HL)", 0, 4, cpu_unimplemented_instruction },
    { "XOR A", 0, 2, cpu_unimplemented_instruction },
    { "OR B", 0, 2, cpu_unimplemented_instruction },                    // 0xB0
    { "OR C", 0, 2, cpu_unimplemented_instruction },
    { "OR D", 0, 2, cpu_unimplemented_instruction },
    { "OR E", 0, 2, cpu_unimplemented_instruction },
    { "OR H", 0, 2, cpu_unimplemented_instruction },
    { "OR L", 0, 2, cpu_unimplemented_instruction },
    { "OR (HL)", 0, 4, cpu_unimplemented_instruction },
    { "OR A", 0, 2, cpu_unimplemented_instruction },
    { "CP B", 0, 2, cpu_unimplemented_instruction },
    { "CP C", 0, 2, cpu_unimplemented_instruction },
    { "CP D", 0, 2, cpu_unimplemented_instruction },
    { "CP E", 0, 2, cpu_unimplemented_instruction },
    { "CP H", 0, 2, cpu_unimplemented_instruction },
    { "CP L", 0, 2, cpu_unimplemented_instruction },
    { "CP (HL)", 0, 4, cpu_unimplemented_instruction },
    { "CP A", 0, 2, cpu_unimplemented_instruction },
    { "RET NZ", 0, 0, cpu_unimplemented_instruction },                 // 0xC0
    { "POP BC", 0, 6, cpu_unimplemented_instruction },
    { "JP NZ, a16", 2, 0, cpu_unimplemented_instruction },
    { "JP 0x%04X", 2, 6, jp_nn },
    { "CALL NZ, a16", 2, 0, cpu_unimplemented_instruction },
    { "PUSH BC", 0, 8, cpu_unimplemented_instruction },
    { "ADD A, d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 0", 0, 8, cpu_unimplemented_instruction },
    { "RET Z", 0, 0, cpu_unimplemented_instruction },
    { "RET", 0, 2, cpu_unimplemented_instruction },
    { "JP Z, a16", 2, 0, cpu_unimplemented_instruction },
    { "NULL (0xCB) NYI", 0, 0, cpu_unimplemented_instruction },
    { "CALL Z, a16", 2, 0, cpu_unimplemented_instruction },
    { "CALL a16", 2, 6, call_nn },
    { "ADC A, d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 1", 0, 8, rst_1 },
    { "RET NC", 0, 0, cpu_unimplemented_instruction },                 // 0xD0
    { "POP DE", 0, 6, cpu_unimplemented_instruction },
    { "JP NC, a16", 2, 0, cpu_unimplemented_instruction },
    { "NULL (0xD3)", 0, 0, cpu_unimplemented_instruction },
    { "CALL NC, a16", 2, 0, cpu_unimplemented_instruction },
    { "PUSH DE", 0, 8, cpu_unimplemented_instruction },
    { "SUB d8", 1, 4, sub_n },
    { "RST 2", 0, 8, cpu_unimplemented_instruction },
    { "RET C", 0, 0, cpu_unimplemented_instruction },
    { "RETI", 0, 8, cpu_unimplemented_instruction },
    { "JP C, a16", 2, 0, cpu_unimplemented_instruction },
    { "NULL (0xDB)", 0, 0, cpu_unimplemented_instruction },
    { "CALL C", 2, 0, cpu_unimplemented_instruction },
    { "NULL (0xDD)", 0, 0, cpu_unimplemented_instruction },
    { "SBC A, d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 3", 0, 8, cpu_unimplemented_instruction },
    { "LD (a8), A", 1, 6, cpu_unimplemented_instruction },             // 0xE0
    { "POP HL", 0, 6, cpu_unimplemented_instruction },
    { "LD (C), A", 0, 4, cpu_unimplemented_instruction },
    { "NULL (0xE3)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xE4)", 0, 0, cpu_unimplemented_instruction },
    { "PUSH HL", 0, 8, cpu_unimplemented_instruction },
    { "AND d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 4", 0, 8, cpu_unimplemented_instruction },
    { "ADD SP, s8", 1, 8, cpu_unimplemented_instruction },
    { "JP HL", 0, 2, cpu_unimplemented_instruction },
    { "LD (a16), A", 0, 8, cpu_unimplemented_instruction },
    { "NULL (0xEB)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xEC)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xED)", 0, 0, cpu_unimplemented_instruction },
    { "XOR d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 5", 0, 8, cpu_unimplemented_instruction },
    { "LD A, (a8)", 1, 6, cpu_unimplemented_instruction },              // 0xF0
    { "POP AF", 0, 6, cpu_unimplemented_instruction },
    { "LD A, (C)", 0, 4, cpu_unimplemented_instruction },
    { "DI", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xF4)", 0, 0, cpu_unimplemented_instruction },
    { "PUSH AF", 0, 8, cpu_unimplemented_instruction },
    { "OR d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 6", 0, 8, cpu_unimplemented_instruction },
    { "LD HL, SP+s8", 1, 6, cpu_unimplemented_instruction },
    { "LD SP, HL", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (a16)", 2, 8, cpu_unimplemented_instruction },
    { "EI", 0, 2, cpu_unimplemented_instruction },
    { "NULL (0xFC)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xFD)", 0, 0, cpu_unimplemented_instruction },
    { "CP d8", 1, 4, cpu_unimplemented_instruction },
    { "RST 7", 0, 8, rst_7 }
};

const struct cpu_instruction cpu_prefix_cb_instructions[256] = {
    { "RLC B", 0, 8, cpu_unimplemented_instruction },          // 0x00
    { "RLC C", 0, 8, cpu_unimplemented_instruction },
    { "RLC D", 0, 8, cpu_unimplemented_instruction },
    { "RLC E", 0, 8, cpu_unimplemented_instruction },
    { "RLC H", 0, 8, cpu_unimplemented_instruction },
    { "RLC L", 0, 8, cpu_unimplemented_instruction },
    { "RLC (HL)", 0, 16, cpu_unimplemented_instruction },
    { "RLC A", 0, 8, cpu_unimplemented_instruction },
    { "RRC B", 0, 8, cpu_unimplemented_instruction },
    { "RRC C", 0, 8, cpu_unimplemented_instruction },
    { "RRC D", 0, 8, cpu_unimplemented_instruction },
    { "RRC E", 0, 8, cpu_unimplemented_instruction },
    { "RRC H", 0, 8, cpu_unimplemented_instruction },
    { "RRC L", 0, 8, cpu_unimplemented_instruction },
    { "RRC (HL)", 0, 16, cpu_unimplemented_instruction },
    { "RRC A", 0, 8, cpu_unimplemented_instruction },
    { "RL B", 0, 8, cpu_unimplemented_instruction },           //0x10 
    { "RL C", 0, 8, cpu_unimplemented_instruction },
    { "RL D", 0, 8, cpu_unimplemented_instruction },
    { "RL E", 0, 8, cpu_unimplemented_instruction },
    { "RL H", 0, 8, cpu_unimplemented_instruction },
    { "RL L", 0, 8, cpu_unimplemented_instruction },
    { "RL (HL)", 0, 16, cpu_unimplemented_instruction },
    { "RL A", 0, 8, cpu_unimplemented_instruction },
    { "RR B", 0, 8, cpu_unimplemented_instruction },
    { "RR C", 0, 8, cpu_unimplemented_instruction },
    { "RR D", 0, 8, cpu_unimplemented_instruction },
    { "RR E", 0, 8, cpu_unimplemented_instruction },
    { "RR H", 0, 8, cpu_unimplemented_instruction },
    { "RR L", 0, 8, cpu_unimplemented_instruction },
    { "RR (HL)", 0, 16, cpu_unimplemented_instruction },
    { "RR A", 0, 8, cpu_unimplemented_instruction },
    { "SLA B", 0, 8, cpu_unimplemented_instruction },          // 0x20
    { "SLA C", 0, 8, cpu_unimplemented_instruction },
    { "SLA D", 0, 8, cpu_unimplemented_instruction },
    { "SLA E", 0, 8, cpu_unimplemented_instruction },
    { "SLA H", 0, 8, cpu_unimplemented_instruction },
    { "SLA L", 0, 8, cpu_unimplemented_instruction },
    { "SLA (HL)", 0, 16, cpu_unimplemented_instruction },
    { "SLA A", 0, 8, cpu_unimplemented_instruction },
    { "SRA B", 0, 8, cpu_unimplemented_instruction },
    { "SRA C", 0, 8, cpu_unimplemented_instruction },
    { "SRA D", 0, 8, cpu_unimplemented_instruction },
    { "SRA E", 0, 8, cpu_unimplemented_instruction },
    { "SRA H", 0, 8, cpu_unimplemented_instruction },
    { "SRA L", 0, 8, cpu_unimplemented_instruction },
    { "SRA (HL)", 0, 16, cpu_unimplemented_instruction },
    { "SRA A", 0, 8, cpu_unimplemented_instruction },
    { "SWAP B", 0, 8, cpu_unimplemented_instruction },          // 0x30
    { "SWAP C", 0, 8, cpu_unimplemented_instruction },
    { "SWAP D", 0, 8, cpu_unimplemented_instruction },
    { "SWAP E", 0, 8, cpu_unimplemented_instruction },
    { "SWAP H", 0, 8, cpu_unimplemented_instruction },
    { "SWAP L", 0, 8, cpu_unimplemented_instruction },
    { "SWAP (HL)", 0, 16, cpu_unimplemented_instruction },
    { "SWAP A", 0, 8, cpu_unimplemented_instruction },
    { "SRL B", 0, 8, cpu_unimplemented_instruction },
    { "SRL C", 0, 8, cpu_unimplemented_instruction },
    { "SRL D", 0, 8, cpu_unimplemented_instruction },
    { "SRL E", 0, 8, cpu_unimplemented_instruction },
    { "SRL H", 0, 8, cpu_unimplemented_instruction },
    { "SRL L", 0, 8, cpu_unimplemented_instruction },
    { "SRL (HL)", 0, 16, cpu_unimplemented_instruction },
    { "SRL A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 0, B", 0, 8, cpu_unimplemented_instruction },           // 0x40
    { "BIT 0, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 0, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 0, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 0, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 0, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 0, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 0, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, B", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 1, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 1, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 2, B", 0, 8, cpu_unimplemented_instruction },           // 0x50
    { "BIT 2, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 2, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 2, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 2, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 2, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 2, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 2, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, B", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 3, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 3, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 4, B", 0, 8, cpu_unimplemented_instruction },            // 0x60
    { "BIT 4, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 4, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 4, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 4, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 4, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 4, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 4, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, B", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 5, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 5, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 6, B", 0, 8, cpu_unimplemented_instruction },           // 0x70
    { "BIT 6, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 6, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 6, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 6, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 6, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 6, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 6, A", 0, 8, cpu_unimplemented_instruction },
    { "BIT 7, B", 0, 8, cpu_unimplemented_instruction },        
    { "BIT 7, C", 0, 8, cpu_unimplemented_instruction },
    { "BIT 7, D", 0, 8, cpu_unimplemented_instruction },
    { "BIT 7, E", 0, 8, cpu_unimplemented_instruction },
    { "BIT 7, H", 0, 8, cpu_unimplemented_instruction },
    { "BIT 7, L", 0, 8, cpu_unimplemented_instruction },
    { "BIT 7, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "BIT 7, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 0, B", 0, 8, cpu_unimplemented_instruction },           // 0x80
    { "RES 0, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 0, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 0, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 0, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 0, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 0, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 0, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, B", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 1, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 1, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 2, B", 0, 8, cpu_unimplemented_instruction },           // 0x90
    { "RES 2, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 2, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 2, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 2, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 2, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 2, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 2, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, B", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 3, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 3, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 4, B", 0, 8, cpu_unimplemented_instruction },           // 0xA0
    { "RES 4, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 4, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 4, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 4, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 4, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 4, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 4, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, B", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 5, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 5, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 6, B", 0, 8, cpu_unimplemented_instruction },           // 0xB0
    { "RES 6, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 6, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 6, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 6, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 6, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 6, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 6, A", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, B", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, C", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, D", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, E", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, H", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, L", 0, 8, cpu_unimplemented_instruction },
    { "RES 7, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "RES 7, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 0, B", 0, 8, cpu_unimplemented_instruction },           // 0xC0
    { "SET 0, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 0, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 0, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 0, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 0, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 0, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 0, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 1, B", 0, 8, cpu_unimplemented_instruction },         
    { "SET 1, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 1, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 1, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 1, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 1, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 1, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 1, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 2, B", 0, 8, cpu_unimplemented_instruction },           // 0xD0         
    { "SET 2, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 2, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 2, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 2, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 2, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 2, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 2, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 3, B", 0, 8, cpu_unimplemented_instruction },         
    { "SET 3, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 3, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 3, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 3, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 3, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 3, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 3, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 4, B", 0, 8, cpu_unimplemented_instruction },           // 0xE0         
    { "SET 4, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 4, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 4, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 4, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 4, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 4, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 4, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 5, B", 0, 8, cpu_unimplemented_instruction },         
    { "SET 5, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 5, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 5, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 5, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 5, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 5, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 5, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 6, B", 0, 8, cpu_unimplemented_instruction },           // 0xF0
    { "SET 6, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 6, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 6, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 6, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 6, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 6, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 6, A", 0, 8, cpu_unimplemented_instruction },
    { "SET 7, B", 0, 8, cpu_unimplemented_instruction },         
    { "SET 7, C", 0, 8, cpu_unimplemented_instruction },
    { "SET 7, D", 0, 8, cpu_unimplemented_instruction },
    { "SET 7, E", 0, 8, cpu_unimplemented_instruction },
    { "SET 7, H", 0, 8, cpu_unimplemented_instruction },
    { "SET 7, L", 0, 8, cpu_unimplemented_instruction },
    { "SET 7, (HL)", 0, 12, cpu_unimplemented_instruction },
    { "SET 7, A", 0, 8, cpu_unimplemented_instruction },
};

void cpu_reset() {
    memset(mmu.sram, 0, sizeof(mmu.sram));
    memset(mmu.vram, 0, sizeof(mmu.vram));
    memset(mmu.oam, 0, sizeof(mmu.oam));
    memset(mmu.wram, 0, sizeof(mmu.wram));
    memset(mmu.hram, 0, sizeof(mmu.hram));
    memcpy(mmu.io, mmu_io_reset, sizeof(mmu.io));

    cpu.stopped = false;
    cpu.emulation_speed = 1;
    cpu.ticks = 0;

    registers.a = 0x01;
    registers.f = 0xb0;
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xd8;
    registers.h = 0x01;
    registers.l = 0x4d;
    registers.sp = 0xfffe;
    registers.pc = 0x100;

    interrupt.master = 1;
    interrupt.enable = 0;
    interrupt.flags = 0;

    gpu_background_palette[0] = display_palette[0];
	gpu_background_palette[1] = display_palette[1];
	gpu_background_palette[2] = display_palette[2];
	gpu_background_palette[3] = display_palette[3];
	
	gpu_sprite_palette[0][0] = display_palette[0];
	gpu_sprite_palette[0][1] = display_palette[1];
	gpu_sprite_palette[0][2] = display_palette[2];
	gpu_sprite_palette[0][3] = display_palette[3];
	
	gpu_sprite_palette[1][0] = display_palette[0];
	gpu_sprite_palette[1][1] = display_palette[1];
	gpu_sprite_palette[1][2] = display_palette[2];
	gpu_sprite_palette[1][3] = display_palette[3];
	
	gpu.control = 0;
	gpu.scroll_x = 0;
	gpu.scroll_y = 0;
	gpu.scanline = 0;
	gpu.tick = 0;

    memset(gpu_tiles, 0, sizeof(gpu_tiles));
	memset(display_framebuffer, 255, sizeof(display_framebuffer));

	
	mmu_write_byte(0xFF05, 0);
	mmu_write_byte(0xFF06, 0);
	mmu_write_byte(0xFF07, 0);
	mmu_write_byte(0xFF10, 0x80);
	mmu_write_byte(0xFF11, 0xBF);
	mmu_write_byte(0xFF12, 0xF3);
	mmu_write_byte(0xFF14, 0xBF);
	mmu_write_byte(0xFF16, 0x3F);
	mmu_write_byte(0xFF17, 0x00);
	mmu_write_byte(0xFF19, 0xBF);
	mmu_write_byte(0xFF1A, 0x7A);
	mmu_write_byte(0xFF1B, 0xFF);
	mmu_write_byte(0xFF1C, 0x9F);
	mmu_write_byte(0xFF1E, 0xBF);
	mmu_write_byte(0xFF20, 0xFF);
	mmu_write_byte(0xFF21, 0x00);
	mmu_write_byte(0xFF22, 0x00);
	mmu_write_byte(0xFF23, 0xBF);
	mmu_write_byte(0xFF24, 0x77);
	mmu_write_byte(0xFF25, 0xF3);
	mmu_write_byte(0xFF26, 0xF1);
	mmu_write_byte(0xFF40, 0x91);
	mmu_write_byte(0xFF42, 0x00);
	mmu_write_byte(0xFF43, 0x00);
	mmu_write_byte(0xFF45, 0x00);
	mmu_write_byte(0xFF47, 0xFC);
	mmu_write_byte(0xFF48, 0xFF);
	mmu_write_byte(0xFF49, 0xFF);
	mmu_write_byte(0xFF4A, 0x00);
	mmu_write_byte(0xFF4B, 0x00);
	mmu_write_byte(0xFFFF, 0x00);
}

void cpu_emulate() {
    unsigned char instruction;
    cpu.operand = 0;
    
    const int MAXCYCLES = 69905; // the maximum amount of cpu cycles per frame
    cpu.ticks = 0;
    while (cpu.ticks < MAXCYCLES && !cpu.stopped) {
        instruction = mmu_read_byte(registers.pc++);
        cpu.last_instruction = instruction;

        if(cpu_instructions[instruction].length == 1) cpu.operand = (unsigned short)mmu_read_byte(registers.pc);
        if(cpu_instructions[instruction].length == 2) cpu.operand = mmu_read_short(registers.pc);
        
        // TODO: add functions to all instruction tables
        // TODO: declare all functions in header for 
        if (cpu_instructions[instruction].function != cpu_unimplemented_instruction) {
            printf("Executing Instruction: 0x%02x: %s, Operand: 0x%04x\n", instruction, cpu_instructions[instruction].mnemonic, cpu.operand);
        }
        
        switch(cpu_instructions[instruction].length) {
            case 0:
                ((void (*)(void))cpu_instructions[instruction].function)();
                break;
            
            case 1:
                ((void (*)(unsigned char))cpu_instructions[instruction].function)((unsigned char)cpu.operand);
                break;
            
            case 2:
                ((void (*)(unsigned short))cpu_instructions[instruction].function)(cpu.operand);
                break;
        }
        
        registers.pc += cpu_instructions[instruction].length;
        cpu.ticks += cpu_instructions[instruction].cycles;
    }
}

void cpu_unimplemented_instruction() {
    registers.sp--;
    char mnemonic[20];
    
    strcpy(mnemonic, cpu_instructions[cpu.last_instruction].mnemonic);
    printf("Unimplemented Instruction: 0x%02x: %s\n", cpu.last_instruction, mnemonic);
    printf("Halting CPU\n");
    cpu.stopped = true;
}

static unsigned char inc(unsigned char value) {
	if((value & 0x0f) == 0x0f) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	value++;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE);
	
	return value;
}

static unsigned char dec(unsigned char value) {
	if(value & 0x0f) FLAGS_CLEAR(FLAGS_HALFCARRY);
	else FLAGS_SET(FLAGS_HALFCARRY);
	
	value--;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_SET(FLAGS_NEGATIVE);
	
	return value;
}

static void add(unsigned char *destination, unsigned char value) {
	unsigned int result = *destination + value;
	
	if(result & 0xff00) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	*destination = (unsigned char)(result & 0xff);
	
	if(*destination) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	if(((*destination & 0x0f) + (value & 0x0f)) > 0x0f) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

static void add2(unsigned short *destination, unsigned short value) {
	unsigned long result = *destination + value;
	
	if(result & 0xffff0000) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	*destination = (unsigned short)(result & 0xffff);
	
	if(((*destination & 0x0f) + (value & 0x0f)) > 0x0f) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	// zero flag left alone
	
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

static void adc(unsigned char value) {
	value += FLAGS_ISCARRY ? 1 : 0;
	
	int result = registers.a + value;
	
	if(result & 0xff00) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if(value == registers.a) FLAGS_SET(FLAGS_ZERO);
	else FLAGS_CLEAR(FLAGS_ZERO);
	
	if(((value & 0x0f) + (registers.a & 0x0f)) > 0x0f) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	FLAGS_SET(FLAGS_NEGATIVE);
	
	registers.a = (unsigned char)(result & 0xff);
}

static void sbc(unsigned char value) {
	value += FLAGS_ISCARRY ? 1 : 0;
	
	FLAGS_SET(FLAGS_NEGATIVE);
	
	if(value > registers.a) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if(value == registers.a) FLAGS_SET(FLAGS_ZERO);
	else FLAGS_CLEAR(FLAGS_ZERO);
	
	if((value & 0x0f) > (registers.a & 0x0f)) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	registers.a -= value;
}

static void sub(unsigned char value) {
	FLAGS_SET(FLAGS_NEGATIVE);
	
	if(value > registers.a) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if((value & 0x0f) > (registers.a & 0x0f)) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	registers.a -= value;
	
	if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
}

static void and(unsigned char value) {
	registers.a &= value;
	
	if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE);
	FLAGS_SET(FLAGS_HALFCARRY);
}

static void or(unsigned char value) {
	registers.a |= value;
	
	if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

static void xor(unsigned char value) {
	registers.a ^= value;
	
	if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

static void cp(unsigned char value) {
	if(registers.a == value) FLAGS_SET(FLAGS_ZERO);
	else FLAGS_CLEAR(FLAGS_ZERO);
	
	if(value > registers.a) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if((value & 0x0f) > (registers.a & 0x0f)) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	FLAGS_SET(FLAGS_NEGATIVE);
}

// 0x00
void cpu_nop() { } //NOP
// 0x01
void cpu_ld_bc_nn(unsigned short operand) { registers.bc = operand; }

//0xc3
void jp_nn(unsigned short operand) { registers.pc = operand; }

//0xcf
void rst_1() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0008; }
void rst_7() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0038; }

void inc_a() { registers.a = inc(registers.a); }
void inc_b() { registers.b = inc(registers.b); }
void inc_c() { registers.c = inc(registers.c); }
void inc_d() { registers.d = inc(registers.d); }
void inc_e() { registers.e = inc(registers.e); }
void inc_h() { registers.h = inc(registers.h); }
void inc_l() { registers.l = inc(registers.l); }

void inc_bc() { registers.bc++; }
void inc_de() { registers.de++; }
void inc_hl() { registers.hl++; }

void inc_sp() { registers.sp++; }

void ld_bcp_a() { mmu_write_byte(registers.bc, registers.a); }

void call_nn(unsigned short operand) { mmu_write_short_to_stack(registers.pc); registers.pc = operand; }

void sub_n(unsigned char operand) { sub(operand); }