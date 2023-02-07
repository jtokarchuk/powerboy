#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "registers.h"
#include "mmu.h"
#include "interrupts.h"

struct cpu cpu;

const struct cpu_instruction cpu_instructions[256] = {
    { "NOP", 1, 1, cpu_unimplemented_instruction },                // 0x00
    { "LD BC, d16", 3, 3, cpu_unimplemented_instruction },         
    { "LD (BC), A", 1, 2, cpu_unimplemented_instruction },         
    { "INC BC", 1, 2, cpu_unimplemented_instruction },             
    { "INC B", 1, 1, cpu_unimplemented_instruction },             
    { "DEC B", 1, 1, cpu_unimplemented_instruction },
    { "LD B, d8", 2, 2, cpu_unimplemented_instruction },
    { "RLCA", 1, 1, cpu_unimplemented_instruction },
    { "LD (a16), SP", 3, 5, cpu_unimplemented_instruction },
    { "ADD HL, BC", 1, 2, cpu_unimplemented_instruction },
    { "LD A, (BC)", 1, 2, cpu_unimplemented_instruction },
    { "DEC BC", 1, 2, cpu_unimplemented_instruction },
    { "INC C", 1, 1, cpu_unimplemented_instruction },
    { "DEC C", 1, 1, cpu_unimplemented_instruction },
    { "LD C, d8", 2, 2, cpu_unimplemented_instruction },
    { "RRCA", 1, 1, cpu_unimplemented_instruction },                         
    { "STOP", 2, 1, cpu_unimplemented_instruction },               // 0x10
    { "LD DE, d16", 3, 3, cpu_unimplemented_instruction },
    { "LD (DE), A", 1, 2, cpu_unimplemented_instruction },
    { "INC DE", 1, 2, cpu_unimplemented_instruction },
    { "INC D", 1, 1, cpu_unimplemented_instruction },
    { "DEC D", 1, 1, cpu_unimplemented_instruction },
    { "LD D, d8", 2, 2, cpu_unimplemented_instruction },
    { "RLA", 1, 1, cpu_unimplemented_instruction },
    { "JR s8", 2, 3, cpu_unimplemented_instruction },
    { "ADD HL, DE", 1, 2, cpu_unimplemented_instruction },
    { "LD A, (DE)", 1, 2, cpu_unimplemented_instruction },
    { "DEC DE", 1, 2, cpu_unimplemented_instruction },
    { "INC E", 1, 1, cpu_unimplemented_instruction },
    { "DEC E", 1, 1, cpu_unimplemented_instruction },
    { "LD E, d8", 2, 2, cpu_unimplemented_instruction },
    { "RRA", 1, 1, cpu_unimplemented_instruction },
    { "JR NZ, s8", 2, 32, cpu_unimplemented_instruction},          // 0x20
    { "LD HL, d16", 3, 3, cpu_unimplemented_instruction },
    { "LD (HL+), A", 1, 2, cpu_unimplemented_instruction },
    { "INC HL", 1, 2, cpu_unimplemented_instruction },
    { "INC H", 1, 1, cpu_unimplemented_instruction },
    { "DEC H", 1, 1, cpu_unimplemented_instruction },
    { "LD H, d8", 2, 2, cpu_unimplemented_instruction },
    { "DAA", 1, 1, cpu_unimplemented_instruction },
    { "JR Z, s8", 2, 32, cpu_unimplemented_instruction},
    { "ADD HL, HL", 1, 2, cpu_unimplemented_instruction },
    { "LD A, (HL+)", 1, 2, cpu_unimplemented_instruction },
    { "DEC HL", 1, 2, cpu_unimplemented_instruction },
    { "INC L", 1, 1, cpu_unimplemented_instruction },
    { "DEC L", 1, 1, cpu_unimplemented_instruction },
    { "LD L, d8", 2, 2, cpu_unimplemented_instruction },
    { "CPL", 1, 1, cpu_unimplemented_instruction },
    { "JR NC, s8", 2, 32, cpu_unimplemented_instruction},          // 0x30
    { "LD SP, d16", 3, 3, cpu_unimplemented_instruction },
    { "LD (HL-), A", 1, 2, cpu_unimplemented_instruction },
    { "INC SP", 1, 2, cpu_unimplemented_instruction },
    { "INC (HL)", 1, 3, cpu_unimplemented_instruction },
    { "DEC (HL)", 1, 3, cpu_unimplemented_instruction},
    { "LD (HL), d8", 2, 3, cpu_unimplemented_instruction },
    { "SCF", 1, 1, cpu_unimplemented_instruction },
    { "JR C, s8", 2, 32, cpu_unimplemented_instruction },
    { "ADD HL, SP", 1, 2, cpu_unimplemented_instruction },
    { "LD A, (HL-)", 1, 2, cpu_unimplemented_instruction },
    { "DEC SP", 1, 2, cpu_unimplemented_instruction },
    { "INC A", 1, 1, cpu_unimplemented_instruction },
    { "DEC A", 1, 1, cpu_unimplemented_instruction },
    { "LD A, d8", 2, 2, cpu_unimplemented_instruction },
    { "CCF", 1, 1, cpu_unimplemented_instruction },
    { "LD B, B", 1, 1, cpu_unimplemented_instruction },            // 0x40
    { "LD B, C", 1, 1, cpu_unimplemented_instruction },
    { "LD B, D", 1, 1, cpu_unimplemented_instruction },
    { "LD B, E", 1, 1, cpu_unimplemented_instruction },
    { "LD B, H", 1, 1, cpu_unimplemented_instruction },
    { "LD B, L", 1, 1, cpu_unimplemented_instruction },
    { "LD B, (HL)", 1, 2, cpu_unimplemented_instruction },
    { "LD B, A", 1, 1, cpu_unimplemented_instruction },
    { "LD C, B", 1, 1, cpu_unimplemented_instruction },
    { "LD C, C", 1, 1, cpu_unimplemented_instruction },
    { "LD C, D", 1, 1, cpu_unimplemented_instruction },
    { "LD C, E", 1, 1, cpu_unimplemented_instruction },
    { "LD C, H", 1, 1, cpu_unimplemented_instruction },
    { "LD C, L", 1, 1, cpu_unimplemented_instruction },
    { "LD C, (HL)", 1, 2, cpu_unimplemented_instruction },
    { "LD C, A", 1, 1, cpu_unimplemented_instruction },
    { "LD D, B", 1, 1, cpu_unimplemented_instruction },             // 0x50
    { "LD D, C", 1, 1, cpu_unimplemented_instruction },
    { "LD D, D", 1, 1, cpu_unimplemented_instruction },
    { "LD D, E", 1, 1, cpu_unimplemented_instruction },
    { "LD D, H", 1, 1, cpu_unimplemented_instruction },
    { "LD D, L", 1, 1, cpu_unimplemented_instruction },
    { "LD D, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "LD D, A", 1, 1, cpu_unimplemented_instruction },
    { "LD E, B", 1, 1, cpu_unimplemented_instruction },
    { "LD E, C", 1, 1, cpu_unimplemented_instruction },
    { "LD E, D", 1, 1, cpu_unimplemented_instruction },
    { "LD E, E", 1, 1, cpu_unimplemented_instruction },
    { "LD E, H", 1, 1, cpu_unimplemented_instruction },
    { "LD E, L", 1, 1, cpu_unimplemented_instruction },
    { "LD E, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "LD E, A", 1, 1, cpu_unimplemented_instruction },
    { "LD H, B", 1, 1, cpu_unimplemented_instruction },              // 0x60
    { "LD H, C", 1, 1, cpu_unimplemented_instruction },
    { "LD H, D", 1, 1, cpu_unimplemented_instruction },
    { "LD H, E", 1, 1, cpu_unimplemented_instruction },
    { "LD H, H", 1, 1, cpu_unimplemented_instruction },
    { "LD H, L", 1, 1, cpu_unimplemented_instruction },
    { "LD H, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "LD H, A", 1, 1, cpu_unimplemented_instruction },
    { "LD L, B", 1, 1, cpu_unimplemented_instruction },
    { "LD L, C", 1, 1, cpu_unimplemented_instruction },
    { "LD L, D", 1, 1, cpu_unimplemented_instruction },
    { "LD L, E", 1, 1, cpu_unimplemented_instruction },
    { "LD L, H", 1, 1, cpu_unimplemented_instruction },
    { "LD L, L", 1, 1, cpu_unimplemented_instruction },
    { "LD L, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "LD L, A", 1, 1, cpu_unimplemented_instruction },
    { "LD (HL), B", 1, 1, cpu_unimplemented_instruction },            // 0x70
    { "LD (HL), C", 1, 1, cpu_unimplemented_instruction },
    { "LD (HL), D", 1, 1, cpu_unimplemented_instruction },
    { "LD (HL), E", 1, 1, cpu_unimplemented_instruction },
    { "LD (HL), H", 1, 1, cpu_unimplemented_instruction },
    { "LD (HL), L", 1, 1, cpu_unimplemented_instruction },
    { "HALT", 1, 1, cpu_unimplemented_instruction },
    { "LD (HL), A", 1, 1, cpu_unimplemented_instruction },
    { "LD A, B", 1, 1, cpu_unimplemented_instruction },
    { "LD A, C", 1, 1, cpu_unimplemented_instruction },
    { "LD A, D", 1, 1, cpu_unimplemented_instruction },
    { "LD A, E", 1, 1, cpu_unimplemented_instruction },
    { "LD A, H", 1, 1, cpu_unimplemented_instruction },
    { "LD A, L", 1, 1, cpu_unimplemented_instruction },
    { "LD A, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "LD A, A", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, B", 1, 1, cpu_unimplemented_instruction },               // 0x80
    { "ADD A, C", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, D", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, E", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, H", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, L", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, A", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, B", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, C", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, D", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, E", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, H", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, L", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, A", 1, 1, cpu_unimplemented_instruction },
    { "SUB B", 1, 1, cpu_unimplemented_instruction },                  // 0x90
    { "SUB C", 1, 1, cpu_unimplemented_instruction },
    { "SUB D", 1, 1, cpu_unimplemented_instruction },
    { "SUB E", 1, 1, cpu_unimplemented_instruction },
    { "SUB H", 1, 1, cpu_unimplemented_instruction },
    { "SUB L", 1, 1, cpu_unimplemented_instruction },
    { "SUB (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SUB A", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, B", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, C", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, D", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, E", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, H", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, L", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, A", 1, 1, cpu_unimplemented_instruction },
    { "AND B", 1, 1, cpu_unimplemented_instruction },                   // 0xA0
    { "AND C", 1, 1, cpu_unimplemented_instruction },
    { "AND D", 1, 1, cpu_unimplemented_instruction },
    { "AND E", 1, 1, cpu_unimplemented_instruction },
    { "AND H", 1, 1, cpu_unimplemented_instruction },
    { "AND L", 1, 1, cpu_unimplemented_instruction },
    { "AND (HL)", 1, 1, cpu_unimplemented_instruction },
    { "AND A", 1, 1, cpu_unimplemented_instruction },
    { "XOR B", 1, 1, cpu_unimplemented_instruction },
    { "XOR C", 1, 1, cpu_unimplemented_instruction },
    { "XOR D", 1, 1, cpu_unimplemented_instruction },
    { "XOR E", 1, 1, cpu_unimplemented_instruction },
    { "XOR H", 1, 1, cpu_unimplemented_instruction },
    { "XOR L", 1, 1, cpu_unimplemented_instruction },
    { "XOR (HL)", 1, 1, cpu_unimplemented_instruction },
    { "XOR A", 1, 1, cpu_unimplemented_instruction },
    { "OR B", 1, 1, cpu_unimplemented_instruction },                    // 0xB0
    { "OR C", 1, 1, cpu_unimplemented_instruction },
    { "OR D", 1, 1, cpu_unimplemented_instruction },
    { "OR E", 1, 1, cpu_unimplemented_instruction },
    { "OR H", 1, 1, cpu_unimplemented_instruction },
    { "OR L", 1, 1, cpu_unimplemented_instruction },
    { "OR (HL)", 1, 1, cpu_unimplemented_instruction },
    { "OR A", 1, 1, cpu_unimplemented_instruction },
    { "CP B", 1, 1, cpu_unimplemented_instruction },
    { "CP C", 1, 1, cpu_unimplemented_instruction },
    { "CP D", 1, 1, cpu_unimplemented_instruction },
    { "CP E", 1, 1, cpu_unimplemented_instruction },
    { "CP H", 1, 1, cpu_unimplemented_instruction },
    { "CP L", 1, 1, cpu_unimplemented_instruction },
    { "CP (HL)", 1, 1, cpu_unimplemented_instruction },
    { "CP A", 1, 1, cpu_unimplemented_instruction },
    { "RET NZ", 1, 1, cpu_unimplemented_instruction },                 // 0xC0
    { "POP BC", 1, 1, cpu_unimplemented_instruction },
    { "JP NZ, a16", 1, 1, cpu_unimplemented_instruction },
    { "JP a16", 1, 1, cpu_unimplemented_instruction },
    { "CALL NZ, a16", 1, 1, cpu_unimplemented_instruction },
    { "PUSH BC", 1, 1, cpu_unimplemented_instruction },
    { "ADD A, d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 0", 1, 1, cpu_unimplemented_instruction },
    { "RET Z", 1, 1, cpu_unimplemented_instruction },
    { "RET", 1, 1, cpu_unimplemented_instruction },
    { "JP Z, a16", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xCB)", 1, 1, cpu_unimplemented_instruction },
    { "CALL Z, a16", 1, 1, cpu_unimplemented_instruction },
    { "CALL a16", 1, 1, cpu_unimplemented_instruction },
    { "ADC A, d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 1", 1, 1, cpu_unimplemented_instruction },
    { "RET NC", 1, 1, cpu_unimplemented_instruction },                 // 0xD0
    { "POP DE", 1, 1, cpu_unimplemented_instruction },
    { "JP NC, a16", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xD3)", 1, 1, cpu_unimplemented_instruction },
    { "CALL NC, a16", 1, 1, cpu_unimplemented_instruction },
    { "PUSH DE", 1, 1, cpu_unimplemented_instruction },
    { "SUB d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 2", 1, 1, cpu_unimplemented_instruction },
    { "RET C", 1, 1, cpu_unimplemented_instruction },
    { "RETI", 1, 1, cpu_unimplemented_instruction },
    { "JP C, a16", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xDB)", 1, 1, cpu_unimplemented_instruction },
    { "CALL C", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xDD)", 1, 1, cpu_unimplemented_instruction },
    { "SBC A, d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 3", 1, 1, cpu_unimplemented_instruction },
    { "LD (a8), A", 1, 1, cpu_unimplemented_instruction },             // 0xE0
    { "POP HL", 1, 1, cpu_unimplemented_instruction },
    { "LD (C), A", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xE3)", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xE4)", 1, 1, cpu_unimplemented_instruction },
    { "PUSH HL", 1, 1, cpu_unimplemented_instruction },
    { "AND d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 4", 1, 1, cpu_unimplemented_instruction },
    { "ADD SP, s8", 1, 1, cpu_unimplemented_instruction },
    { "JP HL", 1, 1, cpu_unimplemented_instruction },
    { "LD (a16), A", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xEB)", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xEC)", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xED)", 1, 1, cpu_unimplemented_instruction },
    { "XOR d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 5", 1, 1, cpu_unimplemented_instruction },
    { "LD A, (a8)", 1, 1, cpu_unimplemented_instruction },              // 0xF0
    { "POP AF", 1, 1, cpu_unimplemented_instruction },
    { "LD A, (C)", 1, 1, cpu_unimplemented_instruction },
    { "DI", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xF4)", 1, 1, cpu_unimplemented_instruction },
    { "PUSH AF", 1, 1, cpu_unimplemented_instruction },
    { "OR d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 6", 1, 1, cpu_unimplemented_instruction },
    { "LD HL, SP+s8", 1, 1, cpu_unimplemented_instruction },
    { "LD SP, HL", 1, 1, cpu_unimplemented_instruction },
    { "LD A, (a16)", 1, 1, cpu_unimplemented_instruction },
    { "EI", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xFC)", 1, 1, cpu_unimplemented_instruction },
    { "NULL (0xFD)", 1, 1, cpu_unimplemented_instruction },
    { "CP d8", 1, 1, cpu_unimplemented_instruction },
    { "RST 7", 1, 1, cpu_unimplemented_instruction }
};

const struct cpu_instruction cpu_prefix_cb_instructions[256] = {
    { "RLC B", 1, 1, cpu_unimplemented_instruction },          // 0x00
    { "RLC C", 1, 1, cpu_unimplemented_instruction },
    { "RLC D", 1, 1, cpu_unimplemented_instruction },
    { "RLC E", 1, 1, cpu_unimplemented_instruction },
    { "RLC H", 1, 1, cpu_unimplemented_instruction },
    { "RLC L", 1, 1, cpu_unimplemented_instruction },
    { "RLC (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RLC A", 1, 1, cpu_unimplemented_instruction },
    { "RRC B", 1, 1, cpu_unimplemented_instruction },
    { "RRC C", 1, 1, cpu_unimplemented_instruction },
    { "RRC D", 1, 1, cpu_unimplemented_instruction },
    { "RRC E", 1, 1, cpu_unimplemented_instruction },
    { "RRC H", 1, 1, cpu_unimplemented_instruction },
    { "RRC L", 1, 1, cpu_unimplemented_instruction },
    { "RRC (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RRC A", 1, 1, cpu_unimplemented_instruction },
    { "RL B", 1, 1, cpu_unimplemented_instruction },           //0x10 
    { "RL C", 1, 1, cpu_unimplemented_instruction },
    { "RL D", 1, 1, cpu_unimplemented_instruction },
    { "RL E", 1, 1, cpu_unimplemented_instruction },
    { "RL H", 1, 1, cpu_unimplemented_instruction },
    { "RL L", 1, 1, cpu_unimplemented_instruction },
    { "RL (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RL A", 1, 1, cpu_unimplemented_instruction },
    { "RR B", 1, 1, cpu_unimplemented_instruction },
    { "RR C", 1, 1, cpu_unimplemented_instruction },
    { "RR D", 1, 1, cpu_unimplemented_instruction },
    { "RR E", 1, 1, cpu_unimplemented_instruction },
    { "RR H", 1, 1, cpu_unimplemented_instruction },
    { "RR L", 1, 1, cpu_unimplemented_instruction },
    { "RR (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RR A", 1, 1, cpu_unimplemented_instruction },
    { "SLA B", 1, 1, cpu_unimplemented_instruction },          // 0x20
    { "SLA C", 1, 1, cpu_unimplemented_instruction },
    { "SLA D", 1, 1, cpu_unimplemented_instruction },
    { "SLA E", 1, 1, cpu_unimplemented_instruction },
    { "SLA H", 1, 1, cpu_unimplemented_instruction },
    { "SLA L", 1, 1, cpu_unimplemented_instruction },
    { "SLA (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SLA A", 1, 1, cpu_unimplemented_instruction },
    { "SRA B", 1, 1, cpu_unimplemented_instruction },
    { "SRA C", 1, 1, cpu_unimplemented_instruction },
    { "SRA D", 1, 1, cpu_unimplemented_instruction },
    { "SRA E", 1, 1, cpu_unimplemented_instruction },
    { "SRA H", 1, 1, cpu_unimplemented_instruction },
    { "SRA L", 1, 1, cpu_unimplemented_instruction },
    { "SRA (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SRA A", 1, 1, cpu_unimplemented_instruction },
    { "SWAP B", 1, 1, cpu_unimplemented_instruction },          // 0x30
    { "SWAP C", 1, 1, cpu_unimplemented_instruction },
    { "SWAP D", 1, 1, cpu_unimplemented_instruction },
    { "SWAP E", 1, 1, cpu_unimplemented_instruction },
    { "SWAP H", 1, 1, cpu_unimplemented_instruction },
    { "SWAP L", 1, 1, cpu_unimplemented_instruction },
    { "SWAP (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SWAP A", 1, 1, cpu_unimplemented_instruction },
    { "SRL B", 1, 1, cpu_unimplemented_instruction },
    { "SRL C", 1, 1, cpu_unimplemented_instruction },
    { "SRL D", 1, 1, cpu_unimplemented_instruction },
    { "SRL E", 1, 1, cpu_unimplemented_instruction },
    { "SRL H", 1, 1, cpu_unimplemented_instruction },
    { "SRL L", 1, 1, cpu_unimplemented_instruction },
    { "SRL (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SRL A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, B", 1, 1, cpu_unimplemented_instruction },           // 0x40
    { "BIT 0, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 0, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, B", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 1, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, B", 1, 1, cpu_unimplemented_instruction },           // 0x50
    { "BIT 2, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 2, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, B", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 3, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, B", 1, 1, cpu_unimplemented_instruction },            // 0x60
    { "BIT 4, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 4, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, B", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 5, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, B", 1, 1, cpu_unimplemented_instruction },           // 0x70
    { "BIT 6, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 6, A", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, B", 1, 1, cpu_unimplemented_instruction },        
    { "BIT 7, C", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, D", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, E", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, H", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, L", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "BIT 7, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, B", 1, 1, cpu_unimplemented_instruction },           // 0x80
    { "RES 0, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 0, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, B", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 1, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, B", 1, 1, cpu_unimplemented_instruction },           // 0x90
    { "RES 2, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 2, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, B", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 3, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, B", 1, 1, cpu_unimplemented_instruction },           // 0xA0
    { "RES 4, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 4, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, B", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 5, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, B", 1, 1, cpu_unimplemented_instruction },           // 0xB0
    { "RES 6, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 6, A", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, B", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, C", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, D", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, E", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, H", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, L", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "RES 7, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, B", 1, 1, cpu_unimplemented_instruction },           // 0xC0
    { "SET 0, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 0, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, B", 1, 1, cpu_unimplemented_instruction },         
    { "SET 1, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 1, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, B", 1, 1, cpu_unimplemented_instruction },           // 0xD0         
    { "SET 2, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 2, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, B", 1, 1, cpu_unimplemented_instruction },         
    { "SET 3, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 3, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, B", 1, 1, cpu_unimplemented_instruction },           // 0xE0         
    { "SET 4, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 4, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, B", 1, 1, cpu_unimplemented_instruction },         
    { "SET 5, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 5, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, B", 1, 1, cpu_unimplemented_instruction },           // 0xF0
    { "SET 6, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 6, A", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, B", 1, 1, cpu_unimplemented_instruction },         
    { "SET 7, C", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, D", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, E", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, H", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, L", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, (HL)", 1, 1, cpu_unimplemented_instruction },
    { "SET 7, A", 1, 1, cpu_unimplemented_instruction },
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
}

void cpu_emulate() {
    unsigned char instruction;
    cpu.operand = 0;
    
    const int MAXCYCLES = 69905; // the maximum amount of cpu cycles per frame
    cpu.ticks = 0;
    
    while (cpu.ticks < MAXCYCLES) {
        if (cpu.stopped) return;

        instruction = mmu_read_byte(registers.pc++);
        
        if(cpu_instructions[instruction].length == 1) cpu.operand = (unsigned short)mmu_read_byte(registers.pc);
        if(cpu_instructions[instruction].length == 2) cpu.operand = mmu_read_short(registers.pc);
        
        registers.pc += cpu_instructions[instruction].length;
        // TODO: add functions to all instruction tables
        // TODO: declare all functions in header for 
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
        
        cpu.ticks += cpu_instructions[instruction].cycles;

        // update timers with cpu_instructions[instruction].cycles
        // update graphics with cpu_instructions[instruction].cycles
        // do interrupts
    }
}

void cpu_unimplemented_instruction() {
    char mnemonic[20];
    registers.pc--;
    unsigned char instruction = mmu_read_byte(registers.pc);
    
    strcpy(mnemonic, cpu_instructions[instruction].mnemonic);
    printf("Unimplemented Instruction: %02x: %s\n", instruction, mnemonic);
    printf("Halting CPU");
    cpu.stopped = true;
}

