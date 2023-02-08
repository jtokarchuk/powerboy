#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "gpu.h"
#include "display.h"
#include "registers.h"
#include "mmu.h"
#include "interrupts.h"
#include "cb.h"

struct cpu cpu;

const struct cpu_instruction cpu_instructions[256] = {
    { "NOP", 0, 2, nop },                // 0x00
    { "LD BC, d16", 2, 6, ld_bc_nn },         
    { "LD (BC), A", 0, 4, ld_bcp_a },         
    { "INC BC", 0, 4, inc_bc },             
    { "INC B", 0, 2, inc_b },             
    { "DEC B", 0, 2, dec_b },
    { "LD B, d8", 1, 4, ld_b_n },
    { "RLCA", 0, 4, cpu_unimplemented_instruction },
    { "LD (a16), SP", 2, 10, cpu_unimplemented_instruction },
    { "ADD HL, BC", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (BC)", 0, 4, cpu_unimplemented_instruction },
    { "DEC BC", 0, 4, cpu_unimplemented_instruction },
    { "INC C", 0, 2, inc_c },
    { "DEC C", 0, 2, cpu_unimplemented_instruction },
    { "LD C, d8", 1, 2, ld_c_n },
    { "RRCA", 0, 4, cpu_unimplemented_instruction },                         
    { "STOP", 1, 2, cpu_unimplemented_instruction },               // 0x10
    { "LD DE, d16", 2, 6, ld_de_nn },
    { "LD (DE), A", 0, 4, cpu_unimplemented_instruction },
    { "INC DE", 0, 4, inc_de },
    { "INC D", 0, 2, inc_d },
    { "DEC D", 0, 2, cpu_unimplemented_instruction },
    { "LD D, d8", 1, 4, ld_d_n },
    { "RLA", 0, 4, cpu_unimplemented_instruction },
    { "JR s8", 1, 10, cpu_unimplemented_instruction },
    { "ADD HL, DE", 0, 4, add_hl_de },
    { "LD A, (DE)", 0, 4, cpu_unimplemented_instruction },
    { "DEC DE", 0, 4, cpu_unimplemented_instruction },
    { "INC E", 0, 2, inc_e },
    { "DEC E", 0, 2, cpu_unimplemented_instruction },
    { "LD E, d8", 1, 4, ld_d_n },
    { "RRA", 0, 4, cpu_unimplemented_instruction },
    { "JR NZ, s8", 1, 2, jr_nz_n },          // 0x20
    { "LD HL, d16", 2, 6, ld_hl_nn },
    { "LD (HL+), A", 0, 4, cpu_unimplemented_instruction },
    { "INC HL", 0, 4, inc_hl },
    { "INC H", 0, 2, inc_h },
    { "DEC H", 0, 2, cpu_unimplemented_instruction },
    { "LD H, d8", 1, 4, ld_h_n },
    { "DAA", 0, 2, cpu_unimplemented_instruction },
    { "JR Z, s8", 1, 0, cpu_unimplemented_instruction},
    { "ADD HL, HL", 0, 4, cpu_unimplemented_instruction },
    { "LD A, (HL+)", 0, 4, cpu_unimplemented_instruction },
    { "DEC HL", 0, 4, cpu_unimplemented_instruction },
    { "INC L", 0, 2, inc_l },
    { "DEC L", 0, 2, cpu_unimplemented_instruction },
    { "LD L, d8", 1, 4, ld_l_n },
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
    { "LD A, d8", 1, 4, ld_a_n },
    { "CCF", 0, 2, ccf },
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
    { "LD E, A", 0, 2, ld_e_a },
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
    { "0xCB Extended", 0, 0, cb_n },
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
    { "LD A, (a8)", 1, 6, ld_ff_ap_n },              // 0xF0
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
void nop() { } //NOP
// 0x01


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

void dec_b() { registers.b = dec(registers.b); }

void ld_bcp_a() { mmu_write_byte(registers.bc, registers.a); }

void ld_e_a() { registers.e = registers.a; }

void ld_a_n(unsigned char operand) { registers.a = operand; }
void ld_b_n(unsigned char operand) { registers.b = operand; }
void ld_c_n(unsigned char operand) { registers.c = operand; }
void ld_d_n(unsigned char operand) { registers.d = operand; }
void ld_e_n(unsigned char operand) { registers.e = operand; }
void ld_h_n(unsigned char operand) { registers.h = operand; }
void ld_l_n(unsigned char operand) { registers.l = operand; }

void ld_bc_nn(unsigned short operand) { registers.bc = operand; }
void ld_de_nn(unsigned short operand) { registers.de = operand; }
void ld_hl_nn(unsigned short operand) { registers.hl = operand; }

// 0xf0
void ld_ff_ap_n(unsigned char operand) { registers.a = mmu_read_byte(0xff00 + operand); }

void call_nn(unsigned short operand) { mmu_write_short_to_stack(registers.pc); registers.pc = operand; }

void add_hl_de() { add2(&registers.hl, registers.de); }

void sub_n(unsigned char operand) { sub(operand); }

void ccf() {
	if(FLAGS_ISCARRY) FLAGS_CLEAR(FLAGS_CARRY);
	else FLAGS_SET(FLAGS_CARRY);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

void jr_nz_n(unsigned char operand) {
	if(FLAGS_ISZERO) cpu.ticks += 8;
	else {
		registers.pc += (signed char)operand;	
		cpu.ticks += 12;
	}
}