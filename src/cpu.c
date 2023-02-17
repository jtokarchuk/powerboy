#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "gpu.h"
#include "registers.h"
#include "mmu.h"
#include "interrupts.h"
#include "cb.h"
#include "keys.h"

struct cpu cpu;

const struct cpu_instruction cpu_instructions[256] = {
    { "NOP", 0, 4, nop },                // 0x00
    { "LD BC, d16", 2, 12, ld_bc_nn },         
    { "LD (BC), A", 0, 8, ld_bcp_a },         
    { "INC BC", 0, 8, inc_bc },             
    { "INC B", 0, 4, inc_b },             
    { "DEC B", 0, 4, dec_b },
    { "LD B, d8", 1, 8, ld_b_n },
    { "RLCA", 0, 4, rlca },
    { "LD (a16), SP", 2, 20, ld_nnp_sp },
    { "ADD HL, BC", 0, 8, add_hl_bc },
    { "LD A, (BC)", 0, 8, ld_a_bcp },
    { "DEC BC", 0, 8, dec_bc },
    { "INC C", 0, 4, inc_c },
    { "DEC C", 0, 4, dec_c },
    { "LD C, d8", 1, 8, ld_c_n },
    { "RRCA", 0, 4, rrca },                         
    { "STOP", 1, 4, stop },               // 0x10
    { "LD DE, d16", 2, 12, ld_de_nn },
    { "LD (DE), A", 0, 8, ld_dep_a },
    { "INC DE", 0, 8, inc_de },
    { "INC D", 0, 4, inc_d },
    { "DEC D", 0, 4, dec_d },
    { "LD D, d8", 1, 8, ld_d_n },
    { "RLA", 0, 4, rla },
    { "JR s8", 1, 12, jr_n },
    { "ADD HL, DE", 0, 8, add_hl_de },
    { "LD A, (DE)", 0, 8, ld_a_dep },
    { "DEC DE", 0, 8, dec_de },
    { "INC E", 0, 4, inc_e },
    { "DEC E", 0, 4, dec_e },
    { "LD E, d8", 1, 8, ld_e_n },
    { "RRA", 0, 4, rra },
    { "JR NZ, s8", 1, 0, jr_nz_n },          // 0x20
    { "LD HL, d16", 2, 12, ld_hl_nn },
    { "LD (HL+), A", 0, 8, ldi_hlp_a },
    { "INC HL", 0, 8, inc_hl },
    { "INC H", 0, 4, inc_h },
    { "DEC H", 0, 4, dec_h },
    { "LD H, d8", 1, 8, ld_h_n },
    { "DAA", 0, 4, daa },
    { "JR Z, s8", 1, 0, jr_z_n},
    { "ADD HL, HL", 0, 8, add_hl_hl },
    { "LD A, (HL+)", 0, 8, ldi_a_hlp },
    { "DEC HL", 0, 8, dec_hl },
    { "INC L", 0, 4, inc_l },
    { "DEC L", 0, 4, dec_l },
    { "LD L, d8", 1, 8, ld_l_n },
    { "CPL", 0, 4, cpl },
    { "JR NC, s8", 1, 0, jr_nc_n},          // 0x30
    { "LD SP, d16", 2, 12, ld_sp_nn },
    { "LD (HL-), A", 0, 8, ldd_hlp_a },
    { "INC SP", 0, 8, inc_sp },
    { "INC (HL)", 0, 12, inc_hlp },
    { "DEC (HL)", 0, 12, dec_hlp},
    { "LD (HL), d8", 1, 12, ld_hlp_n },
    { "SCF", 0, 4, scf },
    { "JR C, s8", 1, 0, jr_c_n },
    { "ADD HL, SP", 0, 8, add_hl_sp },
    { "LD A, (HL-)", 0, 8, ldd_a_hlp },
    { "DEC SP", 0, 8, dec_sp },
    { "INC A", 0, 4, inc_a },
    { "DEC A", 0, 4, dec_a },
    { "LD A, d8", 1, 8, ld_a_n },
    { "CCF", 0, 4, ccf },
    { "LD B, B", 0, 4, nop },            // 0x40
    { "LD B, C", 0, 4, ld_b_c },
    { "LD B, D", 0, 4, ld_b_d },
    { "LD B, E", 0, 4, ld_b_e },
    { "LD B, H", 0, 4, ld_b_h },
    { "LD B, L", 0, 4, ld_b_l },
    { "LD B, (HL)", 0, 8, ld_b_hlp },
    { "LD B, A", 0, 4, ld_b_a },
    { "LD C, B", 0, 4, ld_c_b },
    { "LD C, C", 0, 4, nop },
    { "LD C, D", 0, 4, ld_c_d },
    { "LD C, E", 0, 4, ld_c_e },
    { "LD C, H", 0, 4, ld_c_h },
    { "LD C, L", 0, 4, ld_c_l },
    { "LD C, (HL)", 0, 8, ld_c_hlp },
    { "LD C, A", 0, 4, ld_c_a },
    { "LD D, B", 0, 4, ld_d_b },             // 0x50
    { "LD D, C", 0, 4, ld_d_c },
    { "LD D, D", 0, 4, nop },
    { "LD D, E", 0, 4, ld_d_e },
    { "LD D, H", 0, 4, ld_d_h },
    { "LD D, L", 0, 4, ld_d_l },
    { "LD D, (HL)", 0, 8, ld_d_hlp },
    { "LD D, A", 0, 4, ld_d_a },
    { "LD E, B", 0, 4, ld_e_b },
    { "LD E, C", 0, 4, ld_e_c },
    { "LD E, D", 0, 4, ld_e_d },
    { "LD E, E", 0, 4, nop },
    { "LD E, H", 0, 4, ld_e_h },
    { "LD E, L", 0, 4, ld_e_l },
    { "LD E, (HL)", 0, 8, ld_e_hlp },
    { "LD E, A", 0, 4, ld_e_a },
    { "LD H, B", 0, 4, ld_h_b },              // 0x60
    { "LD H, C", 0, 4, ld_h_c },
    { "LD H, D", 0, 4, ld_h_d },
    { "LD H, E", 0, 4, ld_h_e },
    { "LD H, H", 0, 4, nop },
    { "LD H, L", 0, 4, ld_h_l },
    { "LD H, (HL)", 0, 8, ld_h_hlp },
    { "LD H, A", 0, 4, ld_h_a },
    { "LD L, B", 0, 4, ld_l_b },
    { "LD L, C", 0, 4, ld_l_c },
    { "LD L, D", 0, 4, ld_l_d },
    { "LD L, E", 0, 4, ld_l_e },
    { "LD L, H", 0, 4, ld_l_h },
    { "LD L, L", 0, 4, nop },
    { "LD L, (HL)", 0, 8, ld_l_hlp },
    { "LD L, A", 0, 4, ld_l_a },
    { "LD (HL), B", 0, 8, ld_hlp_b },            // 0x70
    { "LD (HL), C", 0, 8, ld_hlp_c },
    { "LD (HL), D", 0, 8, ld_hlp_d },
    { "LD (HL), E", 0, 8, ld_hlp_e },
    { "LD (HL), H", 0, 8, ld_hlp_h },
    { "LD (HL), L", 0, 8, ld_hlp_l },
    { "HALT", 0, 4, halt },
    { "LD (HL), A", 0, 8, ld_hlp_a },
    { "LD A, B", 0, 4, ld_a_b },
    { "LD A, C", 0, 4, ld_a_c },
    { "LD A, D", 0, 4, ld_a_d },
    { "LD A, E", 0, 4, ld_a_e },
    { "LD A, H", 0, 4, ld_a_h },
    { "LD A, L", 0, 4, ld_a_l },
    { "LD A, (HL)", 0, 8, ld_a_hlp },
    { "LD A, A", 0, 4, nop },
    { "ADD A, B", 0, 4, add_a_b },               // 0x80
    { "ADD A, C", 0, 4, add_a_c },
    { "ADD A, D", 0, 4, add_a_d },
    { "ADD A, E", 0, 4, add_a_e },
    { "ADD A, H", 0, 4, add_a_h },
    { "ADD A, L", 0, 4, add_a_l },
    { "ADD A, (HL)", 0, 8, add_a_hlp },
    { "ADD A, A", 0, 4, add_a_a },
    { "ADC A, B", 0, 4, adc_b },
    { "ADC A, C", 0, 4, adc_c },
    { "ADC A, D", 0, 4, adc_d },
    { "ADC A, E", 0, 4, adc_e },
    { "ADC A, H", 0, 4, adc_h },
    { "ADC A, L", 0, 4, adc_l },
    { "ADC A, (HL)", 0, 8, adc_hl },
    { "ADC A, A", 0, 4, adc_a },
    { "SUB B", 0, 4, sub_b },                  // 0x90
    { "SUB C", 0, 4, sub_c },
    { "SUB D", 0, 4, sub_d },
    { "SUB E", 0, 4, sub_e },
    { "SUB H", 0, 4, sub_h },
    { "SUB L", 0, 4, sub_l },
    { "SUB (HL)", 0, 8, sub_hl },
    { "SUB A", 0, 4, sub_a },
    { "SBC A, B", 0, 4, sbc_b },
    { "SBC A, C", 0, 4, sbc_c },
    { "SBC A, D", 0, 4, sbc_d },
    { "SBC A, E", 0, 4, sbc_e },
    { "SBC A, H", 0, 4, sbc_h },
    { "SBC A, L", 0, 4, sbc_l },
    { "SBC A, (HL)", 0, 8, sbc_hl },
    { "SBC A, A", 0, 4, sbc_a },
    { "AND B", 0, 4, and_b },                   // 0xA0
    { "AND C", 0, 4, and_c },
    { "AND D", 0, 4, and_d },
    { "AND E", 0, 4, and_e },
    { "AND H", 0, 4, and_h },
    { "AND L", 0, 4, and_l },
    { "AND (HL)", 0, 8, and_hl },
    { "AND A", 0, 4, and_a },
    { "XOR B", 0, 4, xor_b },
    { "XOR C", 0, 4, xor_c },
    { "XOR D", 0, 4, xor_d },
    { "XOR E", 0, 4, xor_e },
    { "XOR H", 0, 4, xor_h },
    { "XOR L", 0, 4, xor_l },
    { "XOR (HL)", 0, 8, xor_hl },
    { "XOR A", 0, 4, xor_a },
    { "OR B", 0, 4, or_b },                    // 0xB0
    { "OR C", 0, 4, or_c },
    { "OR D", 0, 4, or_d },
    { "OR E", 0, 4, or_e },
    { "OR H", 0, 4, or_h },
    { "OR L", 0, 4, or_l },
    { "OR (HL)", 0, 8, or_hl },
    { "OR A", 0, 4, or_a },
    { "CP B", 0, 4, cp_b },
    { "CP C", 0, 4, cp_c },
    { "CP D", 0, 4, cp_d },
    { "CP E", 0, 4, cp_e },
    { "CP H", 0, 4, cp_h },
    { "CP L", 0, 4, cp_l },
    { "CP (HL)", 0, 8, cp_hl },
    { "CP A", 0, 4, cp_a },
    { "RET NZ", 0, 0, ret_nz },                 // 0xC0
    { "POP BC", 0, 12, pop_bc },
    { "JP NZ, a16", 2, 0, jp_nz_nn },
    { "JP 0x%04X", 2, 16, jp_nn },
    { "CALL NZ, a16", 2, 0, call_nz_nn },
    { "PUSH BC", 0, 16, push_bc },
    { "ADD A, d8", 1, 8, add_a_n },
    { "RST 0", 0, 16, rst_0 },
    { "RET Z", 0, 0, ret_z },
    { "RET", 0, 16, ret },
    { "JP Z, a16", 2, 0, jp_z_nn },
    { "PREFIX CB", 0, 4, cb_n },
    { "CALL Z, a16", 2, 0, call_z_nn },
    { "CALL a16", 2, 24, call_nn },
    { "ADC A, d8", 1, 8, adc_n },
    { "RST 1", 0, 16, rst_1 },
    { "RET NC", 0, 0, ret_nc },                 // 0xD0
    { "POP DE", 0, 12, pop_de },
    { "JP NC, a16", 2, 0, jp_nc_nn },
    { "NULL (0xD3)", 0, 0, cpu_unimplemented_instruction },
    { "CALL NC, a16", 2, 0, call_nc_nn },
    { "PUSH DE", 0, 16, push_de },
    { "SUB d8", 1, 8, sub_n },
    { "RST 2", 0, 16, rst_2 },
    { "RET C", 0, 0, ret_c },
    { "RETI", 0, 16, interrupts_return },
    { "JP C, a16", 2, 0, jp_c_nn },
    { "NULL (0xDB)", 0, 0, cpu_unimplemented_instruction },
    { "CALL C, a16", 2, 0, call_c_nn },
    { "NULL (0xDD)", 0, 0, cpu_unimplemented_instruction },
    { "SBC A, d8", 1, 8, sbc_n },
    { "RST 3", 0, 16, rst_3 },
    { "LD (a8), A", 1, 12, ld_ff_n_ap },             // 0xE0
    { "POP HL", 0, 12, pop_hl },
    { "LD (C), A", 0, 8, ld_ff_c_a },
    { "NULL (0xE3)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xE4)", 0, 0, cpu_unimplemented_instruction },
    { "PUSH HL", 0, 16, push_hl },
    { "AND d8", 1, 8, and_n },
    { "RST 4", 0, 16, rst_4 },
    { "ADD SP, s8", 1, 16, add_sp_n },
    { "JP HL", 0, 4, jp_hl },
    { "LD (a16), A", 2, 16, ld_nnp_a },
    { "NULL (0xEB)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xEC)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xED)", 0, 0, cpu_unimplemented_instruction },
    { "XOR d8", 1, 8, xor_n },
    { "RST 5", 0, 16, rst_5 },
    { "LD A, (a8)", 1, 12, ld_ff_ap_n },              // 0xF0
    { "POP AF", 0, 12, pop_af },
    { "LD A, (C)", 0, 8, ld_a_ff_c },
    { "DI", 0, 4, di_inst },
    { "NULL (0xF4)", 0, 0, cpu_unimplemented_instruction },
    { "PUSH AF", 0, 16, push_af },
    { "OR d8", 1, 8, or_n },
    { "RST 6", 0, 16, rst_6 },
    { "LD HL, SP+s8", 1, 12, ld_hl_sp_n },
    { "LD SP, HL", 0, 8, ld_sp_hl },
    { "LD A, (a16)", 2, 16, ld_a_nnp },
    { "EI", 0, 4, ei },
    { "NULL (0xFC)", 0, 0, cpu_unimplemented_instruction },
    { "NULL (0xFD)", 0, 0, cpu_unimplemented_instruction },
    { "CP d8", 1, 8, cp_n },
    { "RST 7", 0, 16, rst_7 }
};


void cpu_reset() {
    memset(mmu.sram, 0, sizeof(mmu.sram));
    memset(mmu.vram, 0, sizeof(mmu.vram));
    memset(mmu.oam, 0, sizeof(mmu.oam));
    memset(mmu.wram, 0, sizeof(mmu.wram));
    memset(mmu.hram, 0, sizeof(mmu.hram));
    memcpy(mmu.io, mmu_io_reset, sizeof(mmu.io));

    cpu.instruction = 0;
    cpu.halted = false;
    cpu.halted = false;
    cpu.emulation_speed = 1;
    cpu.ticks = 0;
    cpu.last_ticks = 0;
    cpu.debug_key = false;
    cpu.debug_mode = true;

    registers.a = 0x01;
    registers.f = 0xB0;
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.sp = 0xFFFE;
    registers.pc = 0x100;

    interrupt.master = 1;
    interrupt.enable = 0;
    interrupt.flags = 0;

    gpu_background_palette[0] = gpu_palette[0];
	gpu_background_palette[1] = gpu_palette[1];
	gpu_background_palette[2] = gpu_palette[2];
	gpu_background_palette[3] = gpu_palette[3];
	
	gpu_sprite_palette[0][0] = gpu_palette[0];
	gpu_sprite_palette[0][1] = gpu_palette[1];
	gpu_sprite_palette[0][2] = gpu_palette[2];
	gpu_sprite_palette[0][3] = gpu_palette[3];
	
	gpu_sprite_palette[1][0] = gpu_palette[0];
	gpu_sprite_palette[1][1] = gpu_palette[1];
	gpu_sprite_palette[1][2] = gpu_palette[2];
	gpu_sprite_palette[1][3] = gpu_palette[3];
	
	gpu.control = 0;
	gpu.scroll_x = 0;
	gpu.scroll_y = 0;
	gpu.scanline = 0;
	gpu.tick = 0;

    keys.a = 1;
	keys.b = 1;
	keys.select = 1;
	keys.start = 1;
	keys.right = 1;
	keys.left = 1;
	keys.up = 1;
	keys.down = 1;

	memset(gpu_framebuffer, 255, sizeof(gpu_framebuffer));
	
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
    
    cpu.ticks = 0;

    if (cpu.halted) {
        cpu.ticks += 4; return;
    }
    
    cpu.instruction = 0;
    unsigned short operand = 0;

    
    cpu.instruction = mmu_read_byte(registers.pc++);
    cpu.last_instruction = cpu.instruction;

    if(cpu_instructions[cpu.instruction].length == 1) operand = (unsigned short)mmu_read_byte(registers.pc);
    if(cpu_instructions[cpu.instruction].length == 2) operand = mmu_read_short(registers.pc); 

    //    blarggs test - serial output
    if (mmu_read_byte(0xFF02) == 0x81) {
        char c = mmu_read_byte(0xFF01);
        printf("%c", c);
        mmu_write_byte(0xFF02, 0x0);
    }
    
    registers.pc += cpu_instructions[cpu.instruction].length;
    
    switch(cpu_instructions[cpu.instruction].length) {
        case 0:
            ((void (*)(void))cpu_instructions[cpu.instruction].function)();
            break;
        
        case 1:
            ((void (*)(unsigned char))cpu_instructions[cpu.instruction].function)((unsigned char)operand);
            break;
        
        case 2:
            ((void (*)(unsigned short))cpu_instructions[cpu.instruction].function)(operand);
            break;
    }
    
    
    cpu.ticks += cpu_instructions[cpu.instruction].cycles;
}

void cpu_unimplemented_instruction() {
    registers.pc--;
    char mnemonic[20];
    
    strcpy(mnemonic, cpu_instructions[cpu.last_instruction].mnemonic);
    printf("Unimplemented Instruction: 0x%02x: %s\n", cpu.last_instruction, mnemonic);
    printf("Halting CPU\n");
    cpu.halted = true;
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
	if((value & 0x0f) == 0x00) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	value--;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_SET(FLAGS_NEGATIVE);
	
	return value;
}

static void add(unsigned char *destination, unsigned char value) {
	
    if ((*destination + value) & 0xFF) {
        FLAGS_CLEAR(FLAGS_ZERO);
    }
    else {
        FLAGS_SET(FLAGS_ZERO);
    }

    FLAGS_CLEAR(FLAGS_NEGATIVE);

    if (((*destination & 0x0F) + (value & 0x0F)) > 0x0F) {
        FLAGS_SET(FLAGS_HALFCARRY);
    }
    else {
        FLAGS_CLEAR(FLAGS_HALFCARRY);
    }

    if((*destination + value) > 0xFF) {
        FLAGS_SET(FLAGS_CARRY);
    }
    else {
        FLAGS_CLEAR(FLAGS_CARRY);
    }

    *destination = *destination + value;
	
}

static void add2(unsigned short *destination, unsigned short value) {
	unsigned long result = *destination + value;
	
	if(result & 0xffff0000) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
		
	if((((*destination & 0x0fff) + (value & 0x0fff)) & 0x1000) == 0x1000) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
    *destination = (unsigned short)(result & 0xffff);
	// zero flag left alone
	
	FLAGS_CLEAR(FLAGS_NEGATIVE);
}

static void adc(unsigned char value) {
    int carry = FLAGS_ISCARRY ? 1 : 0;
	
	int result = registers.a + value + carry;
	
	if(result & 0xff00) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if((((registers.a & 0x0f) + (value & 0x0f) + carry) & 0x10) == 0x10) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE);
	
	registers.a = (unsigned char)(result & 0xff);
    if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
    else FLAGS_SET(FLAGS_ZERO);

}

static void sbc(unsigned char value) {
    int carry = FLAGS_ISCARRY ? 1 : 0;
	
    if (value + carry > registers.a) FLAGS_SET(FLAGS_CARRY);
    else FLAGS_CLEAR(FLAGS_CARRY);

    if (((value & 0x0F) + carry) > (registers.a & 0x0F)) FLAGS_SET(FLAGS_HALFCARRY);
    else FLAGS_CLEAR (FLAGS_HALFCARRY);

	registers.a -= value + carry;
    if(registers.a == 0) FLAGS_SET(FLAGS_ZERO);
    else FLAGS_CLEAR(FLAGS_ZERO);
    FLAGS_SET(FLAGS_NEGATIVE);
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

void nop() { } //NOP

void jp_nn(unsigned short operand) { registers.pc = operand; }

void jp_c_nn(unsigned short operand) {
	if(FLAGS_ISCARRY) {
		registers.pc = operand;
		cpu.ticks += 16;
	}
	else cpu.ticks += 12;
}

void jp_nz_nn(unsigned short operand) {
	if(FLAGS_ISZERO) cpu.ticks += 12;
	else {
		registers.pc = operand;
		cpu.ticks += 16;
	}
}

void jr_n(unsigned char operand) {
	registers.pc += (signed char)operand;
}

void jr_nc_n(char operand) {
	if(FLAGS_ISCARRY) cpu.ticks += 8;
	else {
		registers.pc += operand;
		cpu.ticks += 12;
	}
}

void jr_c_n(char operand) {
	if(FLAGS_ISCARRY) {
		registers.pc += operand;
		cpu.ticks += 12;
	}
	else cpu.ticks += 8;
}

//0xcf
void rst_0() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0000; }
void rst_1() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0008; }
void rst_2() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0010; }
void rst_3() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0018; }
void rst_4() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0020; }
void rst_5() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0028; }
void rst_6() { mmu_write_short_to_stack(registers.pc); registers.pc = 0x0030; }
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
void inc_hlp() { mmu_write_byte(registers.hl, inc(mmu_read_byte(registers.hl))); }

void inc_sp() { registers.sp++; }

void dec_a() { registers.a = dec(registers.a); }
void dec_b() { registers.b = dec(registers.b); }
void dec_c() { registers.c = dec(registers.c); }
void dec_d() { registers.d = dec(registers.d); }
void dec_e() { registers.e = dec(registers.e); }
void dec_h() { registers.h = dec(registers.h); }
void dec_l() { registers.l = dec(registers.l); }

void dec_bc() { registers.bc--; }
void dec_de() { registers.de--; }
void dec_hl() { registers.hl--; }
void dec_sp() { registers.sp--; }
void dec_hlp() { mmu_write_byte(registers.hl, dec(mmu_read_byte(registers.hl))); }

void ldd_hlp_a() { mmu_write_byte(registers.hl, registers.a); registers.hl--; }
void ldd_a_hlp() { registers.a = mmu_read_byte(registers.hl--); }
void ld_nnp_a(unsigned short operand) { mmu_write_byte(operand, registers.a); }
void ld_nnp_sp(unsigned short operand) { mmu_write_short(operand, registers.sp); }

void ld_a_nnp(unsigned short operand) { 
    registers.a = mmu_read_byte(operand); 
}

void ld_hlp_b() { mmu_write_byte(registers.hl, registers.b); }
void ld_hlp_c() { mmu_write_byte(registers.hl, registers.c); }
void ld_hlp_d() { mmu_write_byte(registers.hl, registers.d); }
void ld_hlp_e() { mmu_write_byte(registers.hl, registers.e); }
void ld_hlp_h() { mmu_write_byte(registers.hl, registers.h); }
void ld_hlp_l() { mmu_write_byte(registers.hl, registers.l); }
void ld_hlp_a() { mmu_write_byte(registers.hl, registers.a); }

void ld_bcp_a() { mmu_write_byte(registers.bc, registers.a); }
void ld_dep_a() { mmu_write_byte(registers.de, registers.a); }

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

void ld_sp_nn(unsigned short operand) { registers.sp = operand; }
void ld_sp_hl() { registers.sp = registers.hl; }

void ld_hlp_n(unsigned char operand) { mmu_write_byte(registers.hl, operand); }

void ldi_hlp_a() { mmu_write_byte(registers.hl++, registers.a); }
void ldi_a_hlp() { 
    registers.a = mmu_read_byte(registers.hl++); 
}

void ld_a_bcp() { registers.a = mmu_read_byte(registers.bc); }
void ld_a_dep() { registers.a = mmu_read_byte(registers.de); }

void ld_a_hlp() { registers.a = mmu_read_byte(registers.hl); }
void ld_b_hlp() { registers.b = mmu_read_byte(registers.hl); }
void ld_c_hlp() { registers.c = mmu_read_byte(registers.hl); }
void ld_d_hlp() { registers.d = mmu_read_byte(registers.hl); }
void ld_e_hlp() { registers.e = mmu_read_byte(registers.hl); }
void ld_h_hlp() { registers.h = mmu_read_byte(registers.hl); }
void ld_l_hlp() { registers.l = mmu_read_byte(registers.hl); }



void ld_a_b() { registers.a = registers.b; }
void ld_a_c() { registers.a = registers.c; }
void ld_a_d() { registers.a = registers.d; }
void ld_a_e() { registers.a = registers.e; }
void ld_a_h() { registers.a = registers.h; }
void ld_a_l() { registers.a = registers.l; }

void ld_b_a() { registers.b = registers.a; }
void ld_b_c() { registers.b = registers.c; }
void ld_b_d() { registers.b = registers.d; }
void ld_b_e() { registers.b = registers.e; }
void ld_b_h() { registers.b = registers.h; }
void ld_b_l() { registers.b = registers.l; }

void ld_c_a() { registers.c = registers.a; }
void ld_c_b() { registers.c = registers.b; }
void ld_c_d() { registers.c = registers.d; }
void ld_c_e() { registers.c = registers.e; }
void ld_c_h() { registers.c = registers.h; }
void ld_c_l() { registers.c = registers.l; }

void ld_d_a() { registers.d = registers.a; }
void ld_d_b() { registers.d = registers.b; }
void ld_d_c() { registers.d = registers.c; }
void ld_d_e() { registers.d = registers.e; }
void ld_d_h() { registers.d = registers.h; }
void ld_d_l() { registers.d = registers.l; }

void ld_e_a() { registers.e = registers.a; }
void ld_e_b() { registers.e = registers.b; }
void ld_e_c() { registers.e = registers.c; }
void ld_e_d() { registers.e = registers.d; }
void ld_e_h() { registers.e = registers.h; }
void ld_e_l() { registers.e = registers.l; }

void ld_h_a() { registers.h = registers.a; }
void ld_h_b() { registers.h = registers.b; }
void ld_h_c() { registers.h = registers.c; }
void ld_h_d() { registers.h = registers.d; }
void ld_h_e() { registers.h = registers.e; }
void ld_h_l() { registers.h = registers.l; }

void ld_l_a() { registers.l = registers.a; }
void ld_l_b() { registers.l = registers.b; }
void ld_l_c() { registers.l = registers.c; }
void ld_l_d() { registers.l = registers.d; }
void ld_l_e() { registers.l = registers.e; }
void ld_l_h() { registers.l = registers.h; }

void xor_a() { xor(registers.a); }
void xor_b() { xor(registers.b); }
void xor_c() { xor(registers.c); }
void xor_d() { xor(registers.d); }
void xor_e() { xor(registers.e); }
void xor_h() { xor(registers.h); }
void xor_l() { xor(registers.l); }
void xor_n(unsigned char operand) { xor(operand); }

void or_a() { or(registers.a); }
void or_b() { or(registers.b); }
void or_c() { or(registers.c); }
void or_d() { or(registers.d); }
void or_e() { or(registers.e); }
void or_h() { or(registers.h); }
void or_l() { or(registers.l); }
void or_n(unsigned char operand) { or(operand); }

void and_a() { and(registers.a); }
void and_b() { and(registers.b); }
void and_c() { and(registers.c); }
void and_d() { and(registers.d); }
void and_e() { and(registers.e); }
void and_h() { and(registers.h); }
void and_l() { and(registers.l); }

void cp_a() { cp(registers.a); }
void cp_b() { cp(registers.b); }
void cp_c() { cp(registers.c); }
void cp_d() { cp(registers.d); }
void cp_e() { cp(registers.e); }
void cp_h() { cp(registers.h); }
void cp_l() { cp(registers.l); }
void cp_hl() { cp(mmu_read_byte(registers.hl)); }

void and_n(unsigned char operand) {
	registers.a &= operand;
	
	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE);
	FLAGS_SET(FLAGS_HALFCARRY);
	if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
}

void ld_ff_ap_n(unsigned char operand) {
    unsigned char value = mmu_read_byte(0xff00 + operand); 
    registers.a = value; 
}
void ld_ff_n_ap(unsigned char operand) { mmu_write_byte((0xff00 + operand), registers.a); }
void ld_a_ff_c() { registers.a = mmu_read_byte(0xff00 + registers.c); }

void ld_hl_sp_n(unsigned char operand) {
	int result = registers.sp + (signed char)operand;
	
	if((((registers.sp & 0xFF) + (operand & 0xFF)) & 0x100) == 0x100) FLAGS_SET(FLAGS_CARRY);
    else FLAGS_CLEAR (FLAGS_CARRY);
	
	if((((registers.sp & 0x0F) + (operand & 0x0F)) & 0x10) == 0x10) FLAGS_SET(FLAGS_HALFCARRY);
    else FLAGS_CLEAR (FLAGS_HALFCARRY);
	
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE);
	
	registers.hl = (unsigned short)(result & 0xffff);
}

void ld_ff_c_a() { mmu_write_byte(0xff00 + registers.c, registers.a); }

void call_nn(unsigned short operand) { mmu_write_short_to_stack(registers.pc); registers.pc = operand; 
}
void call_c_nn(unsigned short operand) {
	if(FLAGS_ISCARRY) {
		mmu_write_short_to_stack(registers.pc);
		registers.pc = operand;
		cpu.ticks += 24;
	}
	else cpu.ticks += 12;
}

void call_nc_nn(unsigned short operand) {
	if(!FLAGS_ISCARRY) {
		mmu_write_short_to_stack(registers.pc);
		registers.pc = operand;
		cpu.ticks += 24;
	}
	else cpu.ticks += 12;
}

void call_nz_nn(unsigned short operand) {
	if(FLAGS_ISZERO) cpu.ticks += 12;
	else {
		mmu_write_short_to_stack(registers.pc);
		registers.pc = operand;
		cpu.ticks += 24;
	}
}

void call_z_nn(unsigned short operand) {
	if(FLAGS_ISZERO) {
		mmu_write_short_to_stack(registers.pc);
		registers.pc = operand;
		cpu.ticks += 24;
	}
	else cpu.ticks += 12;
}

void add_a_a() { add(&registers.a, registers.a); }
void add_a_b() { add(&registers.a, registers.b); }
void add_a_c() { add(&registers.a, registers.c); }
void add_a_d() { add(&registers.a, registers.d); }
void add_a_e() { add(&registers.a, registers.e); }
void add_a_h() { add(&registers.a, registers.h); }
void add_a_l() { add(&registers.a, registers.l); }
void add_a_n(unsigned char operand) { add(&registers.a, operand); }
void add_a_hlp() { add(&registers.a, mmu_read_byte(registers.hl)); }

void add_hl_bc() { add2(&registers.hl, registers.bc); }
void add_hl_de() { add2(&registers.hl, registers.de); }
void add_hl_hl() { add2(&registers.hl, registers.hl); }
void add_hl_sp() { add2(&registers.hl, registers.sp); }

void add_sp_n(char operand) {
	int result = registers.sp + operand;
	
	if((((registers.sp & 0xFF) + (operand & 0xFF)) & 0x100) == 0x100) FLAGS_SET(FLAGS_CARRY);
    else FLAGS_CLEAR(FLAGS_CARRY);

    if((((registers.sp & 0x0F) + (operand & 0x0F)) & 0x10) == 0x10) FLAGS_SET(FLAGS_HALFCARRY);
    else FLAGS_CLEAR(FLAGS_HALFCARRY);

	if(((registers.sp & 0x0f) + (operand & 0x0f)) > 0x0f) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);

    registers.sp = result & 0xffff;
	
	// _does_ clear the zero flag
	FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE);
}

void adc_b() { adc(registers.b); }
void adc_c() { adc(registers.c); }
void adc_d() { adc(registers.d); }
void adc_e() { adc(registers.e); }
void adc_h() { adc(registers.h); }
void adc_l() { adc(registers.l); }
void adc_a() { adc(registers.a); }
void adc_n(unsigned char operand) { adc(operand); }

void adc_hl() { adc(mmu_read_byte(registers.hl)); }

void sub_n(unsigned char operand) { sub(operand); }
void sub_a() { sub(registers.a); }
void sub_b() { sub(registers.b); }
void sub_c() { sub(registers.c); }
void sub_d() { sub(registers.d); }
void sub_e() { sub(registers.e); }
void sub_h() { sub(registers.h); }
void sub_l() { sub(registers.l); }
void sub_hl() { sub(mmu_read_byte(registers.hl)); }

void sbc_a() { sbc(registers.a); }
void sbc_b() { sbc(registers.b); }
void sbc_c() { sbc(registers.c); }
void sbc_d() { sbc(registers.d); }
void sbc_e() { sbc(registers.e); }
void sbc_h() { sbc(registers.h); }
void sbc_l() { sbc(registers.l); }
void sbc_hl() { sbc(mmu_read_byte(registers.hl)); }
void sbc_n(unsigned char operand) { sbc(operand); }

void and_hl() { and(mmu_read_byte(registers.hl)); }
void or_hl() { or(mmu_read_byte(registers.hl)); }
void xor_hl() { xor(mmu_read_byte(registers.hl)); }

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

void jp_hl() {
	registers.pc = registers.hl;
}

void jp_nc_nn(unsigned short operand) {
	if(!FLAGS_ISCARRY) {
		registers.pc = operand;
		cpu.ticks += 16;
	}
	else cpu.ticks += 12;
}

void jp_z_nn(unsigned short operand) {
	if(FLAGS_ISZERO) {
		registers.pc = operand;
		cpu.ticks += 16;
	}
	else cpu.ticks += 12;
}

void ret() { registers.pc = mmu_read_short_from_stack(); }

void ret_nz() {
	if(FLAGS_ISZERO) cpu.ticks += 8;
	else {
		registers.pc = mmu_read_short_from_stack();
		cpu.ticks += 20;
	}
}

void ret_c() {
	if(FLAGS_ISCARRY) {
		registers.pc = mmu_read_short_from_stack();
		cpu.ticks += 20;
	}
	else cpu.ticks += 8;
}

void ret_nc() {
	if(FLAGS_ISCARRY) cpu.ticks += 8;
	else {
		registers.pc = mmu_read_short_from_stack();
		cpu.ticks += 20;
	}
}

void ret_z() {
	if(FLAGS_ISZERO) {
		registers.pc = mmu_read_short_from_stack();
		cpu.ticks += 20;
	}
	else cpu.ticks += 8;
}

void di_inst() { interrupt.master = 0; }

void cp_n(unsigned char operand) {
	FLAGS_SET(FLAGS_NEGATIVE);
	
	if(registers.a == operand) FLAGS_SET(FLAGS_ZERO);
	else FLAGS_CLEAR(FLAGS_ZERO);
	
	if(operand > registers.a) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if((operand & 0x0f) > (registers.a & 0x0f)) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
}

void ei() { interrupt.master = 1; }

void push_af() { mmu_write_short_to_stack(registers.af); }
void push_bc() { mmu_write_short_to_stack(registers.bc); }
void push_de() { mmu_write_short_to_stack(registers.de); }
void push_hl() { mmu_write_short_to_stack(registers.hl); }

void pop_af() { 
        registers.af = mmu_read_short_from_stack();
        registers.f = registers.f & 0xF0;
    }
void pop_bc() { registers.bc = mmu_read_short_from_stack(); }
void pop_de() { registers.de = mmu_read_short_from_stack(); }
void pop_hl() { registers.hl = mmu_read_short_from_stack(); }

void jr_z_n(unsigned char operand) {

	if(FLAGS_ISZERO) {
		registers.pc += (signed char)operand;
		cpu.ticks += 12;
	}
	else cpu.ticks += 8;

}

void cpl() { registers.a = ~registers.a; FLAGS_SET(FLAGS_NEGATIVE | FLAGS_HALFCARRY); }
void scf() { FLAGS_SET(FLAGS_CARRY); FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY); }

void daa() {
	{
		int s = registers.a;
		
		if(FLAGS_ISSET(FLAGS_NEGATIVE)) {
			if(FLAGS_ISSET(FLAGS_HALFCARRY)) s = (s - 0x06)&0xFF;
			if(FLAGS_ISSET(FLAGS_CARRY)) s -= 0x60;
		}
		else {
			if(FLAGS_ISSET(FLAGS_HALFCARRY) || (s & 0xF) > 9) s += 0x06;
			if(FLAGS_ISSET(FLAGS_CARRY) || s > 0x9F) s += 0x60;
		}
		
		FLAGS_CLEAR(FLAGS_HALFCARRY);
        FLAGS_CLEAR(FLAGS_ZERO);
		
		if((s & 0x100) == 0x100) {
            FLAGS_SET(FLAGS_CARRY);
        } 
		
        s &= 0xFF;

        registers.a = s;
        if (registers.a == 0) FLAGS_SET(FLAGS_ZERO);

	}
}

void rlca() {
	unsigned char carry = (registers.a & 0x80) >> 7;
	if(carry) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	registers.a <<= 1;
	registers.a += carry;
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void rrca() {
	unsigned char carry = registers.a & 0x01;
	if(carry) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	registers.a >>= 1;
	if(carry) registers.a |= 0x80;
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void stop() { cpu.halted = true; }

void halt() {
	cpu.halted = true;
}

void rla() {
	int carry = FLAGS_ISSET(FLAGS_CARRY) ? 1 : 0;
	
	if(registers.a & 0x80) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	registers.a <<= 1;
	registers.a += carry;
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void rra() {
	int carry = (FLAGS_ISSET(FLAGS_CARRY) ? 1 : 0) << 7;
	
	if(registers.a & 0x01) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	registers.a >>= 1;
	registers.a += carry;
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void cpu_print_registers() {
	printf("A: 0x%02x\n", registers.a);
	printf("F: 0x%02x\n", registers.f);
	printf("B: 0x%02x\n", registers.b);
	printf("C: 0x%02x\n", registers.c);
	printf("D: 0x%02x\n", registers.d);
	printf("E: 0x%02x\n", registers.e);
	printf("H: 0x%02x\n", registers.h);
	printf("L: 0x%02x\n", registers.l);
	printf("SP: 0x%04x\n", registers.sp);
	printf("PC: 0x%04x\n", registers.pc);
	printf("IME: 0x%02x\n", interrupt.master);
	printf("IE: 0x%02x\n", interrupt.enable);
	printf("IF: 0x%02x\n", interrupt.flags);
}