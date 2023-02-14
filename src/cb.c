
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "registers.h"
#include "mmu.h"

#include "cb.h"

struct cb_instruction cb_instructions[256] = {
	{ "RLC B", 8, rlc_b },           // 0x00
	{ "RLC C", 8, rlc_c },           // 0x01
	{ "RLC D", 8, rlc_d },           // 0x02
	{ "RLC E", 8, rlc_e },           // 0x03
	{ "RLC H", 8, rlc_h },           // 0x04
	{ "RLC L", 8, rlc_l },           // 0x05
	{ "RLC (HL)", 16, rlc_hlp },      // 0x06
	{ "RLC A", 8, rlc_a },           // 0x07
	{ "RRC B", 8, rrc_b },           // 0x08
	{ "RRC C", 8, rrc_c },           // 0x09
	{ "RRC D", 8, rrc_d },           // 0x0a
	{ "RRC E", 8, rrc_e },           // 0x0b
	{ "RRC H", 8, rrc_h },           // 0x0c
	{ "RRC L", 8, rrc_l },           // 0x0d
	{ "RRC (HL)", 16, rrc_hlp },      // 0x0e
	{ "RRC A", 8, rrc_a },           // 0x0f
	{ "RL B", 8, rl_b },             // 0x10
	{ "RL C", 8, rl_c },             // 0x11
	{ "RL D", 8, rl_d },             // 0x12
	{ "RL E", 8, rl_e },             // 0x13
	{ "RL H", 8, rl_h },             // 0x14
	{ "RL L", 8, rl_l },             // 0x15
	{ "RL (HL)", 16, rl_hlp },        // 0x16
	{ "RL A", 8, rl_a },             // 0x17
	{ "RR B", 8, rr_b },             // 0x18
	{ "RR C", 8, rr_c },             // 0x19
	{ "RR D", 8, rr_d },             // 0x1a
	{ "RR E", 8, rr_e },             // 0x1b
	{ "RR H", 8, rr_h },             // 0x1c
	{ "RR L", 8, rr_l },             // 0x1d
	{ "RR (HL)", 16, rr_hlp },       // 0x1e
	{ "RR A", 8, rr_a },             // 0x1f
	{ "SLA B", 8, sla_b },           // 0x20
	{ "SLA C", 8, sla_c },           // 0x21
	{ "SLA D", 8, sla_d },           // 0x22
	{ "SLA E", 8, sla_e },           // 0x23
	{ "SLA H", 8, sla_h },           // 0x24
	{ "SLA L", 8, sla_l },           // 0x25
	{ "SLA (HL)", 16, sla_hlp },     // 0x26
	{ "SLA A", 8, sla_a },           // 0x27
	{ "SRA B", 8, sra_b },           // 0x28
	{ "SRA C", 8, sra_c },           // 0x29
	{ "SRA D", 8, sra_d },           // 0x2a
	{ "SRA E", 8, sra_e },           // 0x2b
	{ "SRA H", 8, sra_h },           // 0x2c
	{ "SRA L", 8, sra_l },           // 0x2d
	{ "SRA (HL)", 16, sra_hlp },     // 0x2e
	{ "SRA A", 8, sra_a },           // 0x2f
	{ "SWAP B", 8, swap_b },         // 0x30
	{ "SWAP C", 8, swap_c },         // 0x31
	{ "SWAP D", 8, swap_d },         // 0x32
	{ "SWAP E", 8, swap_e },         // 0x33
	{ "SWAP H", 8, swap_h },         // 0x34
	{ "SWAP L", 8, swap_l },         // 0x35
	{ "SWAP (HL)", 16, swap_hlp },   // 0x36
	{ "SWAP A", 8, swap_a },         // 0x37
	{ "SRL B", 8, srl_b },           // 0x38
	{ "SRL C", 8, srl_c },           // 0x39
	{ "SRL D", 8, srl_d },           // 0x3a
	{ "SRL E", 8, srl_e },           // 0x3b
	{ "SRL H", 8, srl_h },           // 0x3c
	{ "SRL L", 8, srl_l },           // 0x3d
	{ "SRL (HL)", 16, srl_hlp },      // 0x3e
	{ "SRL A", 8, srl_a },           // 0x3f
	{ "BIT 0, B", 8, bit_0_b },      // 0x40
	{ "BIT 0, C", 8, bit_0_c },      // 0x41
	{ "BIT 0, D", 8, bit_0_d },      // 0x42
	{ "BIT 0, E", 8, bit_0_e },      // 0x43
	{ "BIT 0, H", 8, bit_0_h },      // 0x44
	{ "BIT 0, L", 8, bit_0_l },      // 0x45
	{ "BIT 0, (HL)", 12, bit_0_hlp }, // 0x46
	{ "BIT 0, A", 8, bit_0_a },      // 0x47
	{ "BIT 1, B", 8, bit_1_b },      // 0x48
	{ "BIT 1, C", 8, bit_1_c },      // 0x49
	{ "BIT 1, D", 8, bit_1_d },      // 0x4a
	{ "BIT 1, E", 8, bit_1_e },      // 0x4b
	{ "BIT 1, H", 8, bit_1_h },      // 0x4c
	{ "BIT 1, L", 8, bit_1_l },      // 0x4d
	{ "BIT 1, (HL)", 12, bit_1_hlp }, // 0x4e
	{ "BIT 1, A", 8, bit_1_a },      // 0x4f
	{ "BIT 2, B", 8, bit_2_b },      // 0x50
	{ "BIT 2, C", 8, bit_2_c },      // 0x51
	{ "BIT 2, D", 8, bit_2_d },      // 0x52
	{ "BIT 2, E", 8, bit_2_e },      // 0x53
	{ "BIT 2, H", 8, bit_2_h },      // 0x54
	{ "BIT 2, L", 8, bit_2_l },      // 0x55
	{ "BIT 2, (HL)", 12, bit_2_hlp }, // 0x56
	{ "BIT 2, A", 8, bit_2_a },      // 0x57
	{ "BIT 3, B", 8, bit_3_b },      // 0x58
	{ "BIT 3, C", 8, bit_3_c },      // 0x59
	{ "BIT 3, D", 8, bit_3_d },      // 0x5a
	{ "BIT 3, E", 8, bit_3_e },      // 0x5b
	{ "BIT 3, H", 8, bit_3_h },      // 0x5c
	{ "BIT 3, L", 8, bit_3_l },      // 0x5d
	{ "BIT 3, (HL)", 12, bit_3_hlp }, // 0x5e
	{ "BIT 3, A", 8, bit_3_a },      // 0x5f
	{ "BIT 4, B", 8, bit_4_b },      // 0x60
	{ "BIT 4, C", 8, bit_4_c },      // 0x61
	{ "BIT 4, D", 8, bit_4_d },      // 0x62
	{ "BIT 4, E", 8, bit_4_e },      // 0x63
	{ "BIT 4, H", 8, bit_4_h },      // 0x64
	{ "BIT 4, L", 8, bit_4_l },      // 0x65
	{ "BIT 4, (HL)", 12, bit_4_hlp }, // 0x66
	{ "BIT 4, A", 8, bit_4_a },      // 0x67
	{ "BIT 5, B", 8, bit_5_b },      // 0x68
	{ "BIT 5, C", 8, bit_5_c },      // 0x69
	{ "BIT 5, D", 8, bit_5_d },      // 0x6a
	{ "BIT 5, E", 8, bit_5_e },      // 0x6b
	{ "BIT 6, H", 8, bit_5_h },      // 0x6c
	{ "BIT 6, L", 8, bit_5_l },      // 0x6d
	{ "BIT 5, (HL)", 12, bit_5_hlp }, // 0x6e
	{ "BIT 5, A", 8, bit_5_a },      // 0x6f
	{ "BIT 6, B", 8, bit_6_b },      // 0x70
	{ "BIT 6, C", 8, bit_6_c },      // 0x71
	{ "BIT 6, D", 8, bit_6_d },      // 0x72
	{ "BIT 6, E", 8, bit_6_e },      // 0x73
	{ "BIT 6, H", 8, bit_6_h },      // 0x74
	{ "BIT 6, L", 8, bit_6_l },      // 0x75
	{ "BIT 6, (HL)", 12, bit_6_hlp }, // 0x76
	{ "BIT 6, A", 8, bit_6_a },      // 0x77
	{ "BIT 7, B", 8, bit_7_b },      // 0x78
	{ "BIT 7, C", 8, bit_7_c },      // 0x79
	{ "BIT 7, D", 8, bit_7_d },      // 0x7a
	{ "BIT 7, E", 8, bit_7_e },      // 0x7b
	{ "BIT 7, H", 8, bit_7_h },      // 0x7c
	{ "BIT 7, L", 8, bit_7_l },      // 0x7d
	{ "BIT 7, (HL)", 12, bit_7_hlp }, // 0x7e
	{ "BIT 7, A", 8, bit_7_a },      // 0x7f
	{ "RES 0, B", 8, res_0_b },      // 0x80
	{ "RES 0, C", 8, res_0_c },      // 0x81
	{ "RES 0, D", 8, res_0_d },      // 0x82
	{ "RES 0, E", 8, res_0_e },      // 0x83
	{ "RES 0, H", 8, res_0_h },      // 0x84
	{ "RES 0, L", 8, res_0_l },      // 0x85
	{ "RES 0, (HL)", 12, res_0_hlp }, // 0x86
	{ "RES 0, A", 8, res_0_a },      // 0x87
	{ "RES 1, B", 8, res_1_b },      // 0x88
	{ "RES 1, C", 8, res_1_c },      // 0x89
	{ "RES 1, D", 8, res_1_d },      // 0x8a
	{ "RES 1, E", 8, res_1_e },      // 0x8b
	{ "RES 1, H", 8, res_1_h },      // 0x8c
	{ "RES 1, L", 8, res_1_l },      // 0x8d
	{ "RES 1, (HL)", 12, res_1_hlp }, // 0x8e
	{ "RES 1, A", 8, res_1_a },      // 0x8f
	{ "RES 2, B", 8, res_2_b },      // 0x90
	{ "RES 2, C", 8, res_2_c },      // 0x91
	{ "RES 2, D", 8, res_2_d },      // 0x92
	{ "RES 2, E", 8, res_2_e },      // 0x93
	{ "RES 2, H", 8, res_2_h },      // 0x94
	{ "RES 2, L", 8, res_2_l },      // 0x95
	{ "RES 2, (HL)", 12, res_2_hlp }, // 0x96
	{ "RES 2, A", 8, res_2_a },      // 0x97
	{ "RES 3, B", 8, res_3_b },      // 0x98
	{ "RES 3, C", 8, res_3_c },      // 0x99
	{ "RES 3, D", 8, res_3_d },      // 0x9a
	{ "RES 3, E", 8, res_3_e },      // 0x9b
	{ "RES 3, H", 8, res_3_h },      // 0x9c
	{ "RES 3, L", 8, res_3_l },      // 0x9d
	{ "RES 3, (HL)", 12, res_3_hlp }, // 0x9e
	{ "RES 3, A", 8, res_3_a },      // 0x9f
	{ "RES 4, B", 8, res_4_b },      // 0xa0
	{ "RES 4, C", 8, res_4_c },      // 0xa1
	{ "RES 4, D", 8, res_4_d },      // 0xa2
	{ "RES 4, E", 8, res_4_e },      // 0xa3
	{ "RES 4, H", 8, res_4_h },      // 0xa4
	{ "RES 4, L", 8, res_4_l },      // 0xa5
	{ "RES 4, (HL)", 12, res_4_hlp }, // 0xa6
	{ "RES 4, A", 8, res_4_a },      // 0xa7
	{ "RES 5, B", 8, res_5_b },      // 0xa8
	{ "RES 5, C", 8, res_5_c },      // 0xa9
	{ "RES 5, D", 8, res_5_d },      // 0xaa
	{ "RES 5, E", 8, res_5_e },      // 0xab
	{ "RES 5, H", 8, res_5_h },      // 0xac
	{ "RES 5, L", 8, res_5_l },      // 0xad
	{ "RES 5, (HL)", 12, res_5_hlp }, // 0xae
	{ "RES 5, A", 8, res_5_a },      // 0xaf
	{ "RES 6, B", 8, res_6_b },      // 0xb0
	{ "RES 6, C", 8, res_6_c },      // 0xb1
	{ "RES 6, D", 8, res_6_d },      // 0xb2
	{ "RES 6, E", 8, res_6_e },      // 0xb3
	{ "RES 6, H", 8, res_6_h },      // 0xb4
	{ "RES 6, L", 8, res_6_l },      // 0xb5
	{ "RES 6, (HL)", 12, res_6_hlp }, // 0xb6
	{ "RES 6, A", 8, res_6_a },      // 0xb7
	{ "RES 7, B", 8, res_7_b },      // 0xb8
	{ "RES 7, C", 8, res_7_c },      // 0xb9
	{ "RES 7, D", 8, res_7_d },      // 0xba
	{ "RES 7, E", 8, res_7_e },      // 0xbb
	{ "RES 7, H", 8, res_7_h },      // 0xbc
	{ "RES 7, L", 8, res_7_l },      // 0xbd
	{ "RES 7, (HL)", 12, res_7_hlp }, // 0xbe
	{ "RES 7, A", 8, res_7_a },      // 0xbf
	{ "SET 0, B", 8, set_0_b },      // 0xc0
	{ "SET 0, C", 8, set_0_c },      // 0xc1
	{ "SET 0, D", 8, set_0_d },      // 0xc2
	{ "SET 0, E", 8, set_0_e },      // 0xc3
	{ "SET 0, H", 8, set_0_h },      // 0xc4
	{ "SET 0, L", 8, set_0_l },      // 0xc5
	{ "SET 0, (HL)", 12, set_0_hlp }, // 0xc6
	{ "SET 0, A", 8, set_0_a },      // 0xc7
	{ "SET 1, B", 8, set_1_b },      // 0xc8
	{ "SET 1, C", 8, set_1_c },      // 0xc9
	{ "SET 1, D", 8, set_1_d },      // 0xca
	{ "SET 1, E", 8, set_1_e },      // 0xcb
	{ "SET 1, H", 8, set_1_h },      // 0xcc
	{ "SET 1, L", 8, set_1_l },      // 0xcd
	{ "SET 1, (HL)", 12, set_1_hlp }, // 0xce
	{ "SET 1, A", 8, set_1_a },      // 0xcf
	{ "SET 2, B", 8, set_2_b },      // 0xd0
	{ "SET 2, C", 8, set_2_c },      // 0xd1
	{ "SET 2, D", 8, set_2_d },      // 0xd2
	{ "SET 2, E", 8, set_2_e },      // 0xd3
	{ "SET 2, H", 8, set_2_h },      // 0xd4
	{ "SET 2, L", 8, set_2_l },      // 0xd5
	{ "SET 2, (HL)", 12, set_2_hlp }, // 0xd6
	{ "SET 2, A", 8, set_2_a },      // 0xd7
	{ "SET 3, B", 8, set_3_b },      // 0xd8
	{ "SET 3, C", 8, set_3_c },      // 0xd9
	{ "SET 3, D", 8, set_3_d },      // 0xda
	{ "SET 3, E", 8, set_3_e },      // 0xdb
	{ "SET 3, H", 8, set_3_h },      // 0xdc
	{ "SET 3, L", 8, set_3_l },      // 0xdd
	{ "SET 3, (HL)", 12, set_3_hlp }, // 0xde
	{ "SET 3, A", 8, set_3_a },      // 0xdf
	{ "SET 4, B", 8, set_4_b },      // 0xe0
	{ "SET 4, C", 8, set_4_c },      // 0xe1
	{ "SET 4, D", 8, set_4_d },      // 0xe2
	{ "SET 4, E", 8, set_4_e },      // 0xe3
	{ "SET 4, H", 8, set_4_h },      // 0xe4
	{ "SET 4, L", 8, set_4_l },      // 0xe5
	{ "SET 4, (HL)", 12, set_4_hlp }, // 0xe6
	{ "SET 4, A", 8, set_4_a },      // 0xe7
	{ "SET 5, B", 8, set_5_b },      // 0xe8
	{ "SET 5, C", 8, set_5_c },      // 0xe9
	{ "SET 5, D", 8, set_5_d },      // 0xea
	{ "SET 5, E", 8, set_5_e },      // 0xeb
	{ "SET 5, H", 8, set_5_h },      // 0xec
	{ "SET 5, L", 8, set_5_l },      // 0xed
	{ "SET 5, (HL)", 12, set_5_hlp }, // 0xee
	{ "SET 5, A", 8, set_5_a },      // 0xef
	{ "SET 6, B", 8, set_6_b },      // 0xf0
	{ "SET 6, C", 8, set_6_c },      // 0xf1
	{ "SET 6, D", 8, set_6_d },      // 0xf2
	{ "SET 6, E", 8, set_6_e },      // 0xf3
	{ "SET 6, H", 8, set_6_h },      // 0xf4
	{ "SET 6, L", 8, set_6_l },      // 0xf5
	{ "SET 6, (HL)", 12, set_6_hlp }, // 0xf6
	{ "SET 6, A", 8, set_6_a },      // 0xf7
	{ "SET 7, B", 8, set_7_b },      // 0xf8
	{ "SET 7, C", 8, set_7_c },      // 0xf9
	{ "SET 7, D", 8, set_7_d },      // 0xfa
	{ "SET 7, E", 8, set_7_e },      // 0xfb
	{ "SET 7, H", 8, set_7_h },      // 0xfc
	{ "SET 7, L", 8, set_7_l },      // 0xfd
	{ "SET 7, (HL)", 12, set_7_hlp }, // 0xfe
	{ "SET 7, A", 8, set_7_a },      // 0xff
};


void cb_n(unsigned char instruction) {
	instruction = mmu_read_byte(registers.pc++);
	cb_instructions[instruction].execute();
	cpu.ticks += cb_instructions[instruction].cycles;
}

static unsigned char rlc(unsigned char value) {
	int carry = (value & 0x80) >> 7;
	
	if(value & 0x80) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	value <<= 1;
	value += carry;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	
	return value;
}

static unsigned char rrc(unsigned char value) {
	int carry = value & 0x01;
	
	value >>= 1;
	
	if(carry) {
		FLAGS_SET(FLAGS_CARRY);
		value |= 0x80;
	}
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	
	return value;
}

static unsigned char rl(unsigned char value) {
	int carry = FLAGS_ISSET(FLAGS_CARRY) ? 1 : 0;
	
	if(value & 0x80) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	value <<= 1;
	value += carry;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	
	return value;
}

static unsigned char rr(unsigned char value) {
	
	int carry = FLAGS_ISSET(FLAGS_CARRY) ? 1 : 0;

	if(value & 0x01) {
		FLAGS_SET(FLAGS_CARRY);
	}
	else {
		FLAGS_CLEAR(FLAGS_CARRY);
	} 
	//printf("value before: %02x\n", value);
	value >>= 1;
	//printf("value after but before carry: %02x\n", value);
	//printf("Carry is: %02d\n", carry);
	if(carry) {
		SET_BIT((value), 7);
	} 
	else {
		RESET_BIT((value), 7);
	}
	
	//printf("value after carry: %02x\n\n", value);

	if(value) { 
		FLAGS_CLEAR(FLAGS_ZERO);
	}
	else {
		FLAGS_SET(FLAGS_ZERO);
	}
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

static unsigned char sla(unsigned char value) {
	if(value & 0x80) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	value <<= 1;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	
	return value;
}

static unsigned char sra(unsigned char value) {
	int msb = value & 0x80;

	if (value & 0x01) {
		FLAGS_SET(FLAGS_CARRY);
	} 
	else {
		FLAGS_CLEAR(FLAGS_CARRY);
	}

	value >>=1;
	value |= msb;
	
	if (value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	return value;
}

static unsigned char swap(unsigned char value) {
	value = ((value & 0xf) << 4) | ((value & 0xf0) >> 4);
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY | FLAGS_CARRY);
	
	return value;
}

static unsigned char srl(unsigned char value) {
	if(value & 0x01) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	value >>= 1;
	
	if(value) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
		
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
	
	return value;
}

static void bit(unsigned char bit, unsigned char value) {
	if(value & bit) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE);
	FLAGS_SET(FLAGS_HALFCARRY);
}

static unsigned char set(unsigned char bit, unsigned char value) {
	value |= bit;
	return value;
}

// 0x00
void rlc_b() { registers.b = rlc(registers.b); }

// 0x01
void rlc_c() { registers.c = rlc(registers.c); }

// 0x02
void rlc_d() { registers.d = rlc(registers.d); }

// 0x03
void rlc_e() { registers.e = rlc(registers.e); }

// 0x04
void rlc_h() { registers.h = rlc(registers.h); }

// 0x05
void rlc_l() { registers.l = rlc(registers.l); }

// 0x06
void rlc_hlp() { mmu_write_byte(registers.hl, rlc(mmu_read_byte(registers.hl))); }

// 0x07
void rlc_a() { registers.a = rlc(registers.a); }

// 0x08
void rrc_b() { registers.b = rrc(registers.b); }

// 0x09
void rrc_c() { registers.c = rrc(registers.c); }

// 0x0a
void rrc_d() { registers.d = rrc(registers.d); }

// 0x0b
void rrc_e() { registers.e = rrc(registers.e); }

// 0x0c
void rrc_h() { registers.h = rrc(registers.h); }

// 0x0d
void rrc_l() { registers.l = rrc(registers.l); }

// 0x0e
void rrc_hlp() { mmu_write_byte(registers.hl, rrc(mmu_read_byte(registers.hl))); }

// 0x0f
void rrc_a() { registers.a = rrc(registers.a); }

// 0x10
void rl_b() { registers.b = rl(registers.b); }

// 0x11
void rl_c() { registers.c = rl(registers.c); }

// 0x12
void rl_d() { registers.d = rl(registers.d); }

// 0x13
void rl_e() { registers.e = rl(registers.e); }

// 0x14
void rl_h() { registers.h = rl(registers.h); }

// 0x15
void rl_l() { registers.l = rl(registers.l); }

// 0x16
void rl_hlp() { mmu_write_byte(registers.hl, rl(mmu_read_byte(registers.hl))); }

// 0x17
void rl_a() { registers.a = rl(registers.a); }

// 0x18
void rr_b() { registers.b = rr(registers.b); }

// 0x19
void rr_c() { registers.c = rr(registers.c); }

// 0x1a
void rr_d() { registers.d = rr(registers.d); }

// 0x1b
void rr_e() { registers.e = rr(registers.e); }

// 0x1c
void rr_h() { registers.h = rr(registers.h); }

// 0x1d
void rr_l() { registers.l = rr(registers.l); }

// 0x1e
void rr_hlp() { mmu_write_byte(registers.hl, rr(mmu_read_byte(registers.hl))); }

// 0x1f
void rr_a() { registers.a = rr(registers.a); }

// 0x20
void sla_b() { registers.b = sla(registers.b); }

// 0x21
void sla_c() { registers.c = sla(registers.c); }

// 0x22
void sla_d() { registers.d = sla(registers.d); }

// 0x23
void sla_e() { registers.e = sla(registers.e); }

// 0x24
void sla_h() { registers.h = sla(registers.h); }

// 0x25
void sla_l() { registers.l = sla(registers.l); }

// 0x26
void sla_hlp() { mmu_write_byte(registers.hl, sla(mmu_read_byte(registers.hl))); }

// 0x27
void sla_a() { registers.a = sla(registers.a); }

// 0x28
void sra_b() { registers.b = sra(registers.b); }

// 0x29
void sra_c() { registers.c = sra(registers.c); }

// 0x2a
void sra_d() { registers.d = sra(registers.d); }

// 0x2b
void sra_e() { registers.e = sra(registers.e); }

// 0x2c
void sra_h() { registers.h = sra(registers.h); }

// 0x2d
void sra_l() { registers.l = sra(registers.l); }

// 0x2e
void sra_hlp() { 
	unsigned char value = mmu_read_byte(registers.hl);
	value = sra(value); 
	mmu_write_byte(registers.hl, value); }

// 0x2f
void sra_a() { registers.a = sra(registers.a); }

// 0x30
void swap_b() { registers.b = swap(registers.b); }

// 0x31
void swap_c() { registers.c = swap(registers.c); }

// 0x32
void swap_d() { registers.d = swap(registers.d); }

// 0x33
void swap_e() { registers.e = swap(registers.e); }

// 0x34
void swap_h() { registers.h = swap(registers.h); }

// 0x35
void swap_l() { registers.l = swap(registers.l); }

// 0x36
void swap_hlp() { mmu_write_byte(registers.hl, swap(mmu_read_byte(registers.hl))); }

// 0x37
void swap_a() { registers.a = swap(registers.a); }

// 0x38
void srl_b() { registers.b = srl(registers.b); }

// 0x39
void srl_c() { registers.c = srl(registers.c); }

// 0x3a
void srl_d() { registers.d = srl(registers.d); }

// 0x3b
void srl_e() { registers.e = srl(registers.e); }

// 0x3c
void srl_h() { registers.h = srl(registers.h); }

// 0x3d
void srl_l() { registers.l = srl(registers.l); }

// 0x3e
void srl_hlp() { mmu_write_byte(registers.hl, srl(mmu_read_byte(registers.hl))); }

// 0x3f
void srl_a() {
	if(registers.a & 0x01) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	registers.a >>= 1;
	
	if(registers.a) FLAGS_CLEAR(FLAGS_ZERO);
	else FLAGS_SET(FLAGS_ZERO);
	
	FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

// 0x40
void bit_0_b() { bit(1 << 0, registers.b); }

// 0x41
void bit_0_c() { bit(1 << 0, registers.c); }

// 0x42
void bit_0_d() { bit(1 << 0, registers.d); }

// 0x43
void bit_0_e() { bit(1 << 0, registers.e); }

// 0x44
void bit_0_h() { bit(1 << 0, registers.h); }

// 0x45
void bit_0_l() { bit(1 << 0, registers.l); }

// 0x46
void bit_0_hlp() { bit(1 << 0, mmu_read_byte(registers.hl)); }

// 0x47
void bit_0_a() { bit(1 << 0, registers.a); }

// 0x48
void bit_1_b() { bit(1 << 1, registers.b); }

// 0x49
void bit_1_c() { bit(1 << 1, registers.c); }

// 0x4a
void bit_1_d() { bit(1 << 1, registers.d); }

// 0x4b
void bit_1_e() { bit(1 << 1, registers.e); }

// 0x4c
void bit_1_h() { bit(1 << 1, registers.h); }

// 0x4d
void bit_1_l() { bit(1 << 1, registers.l); }

// 0x4e
void bit_1_hlp() { bit(1 << 1, mmu_read_byte(registers.hl)); }

// 0x4f
void bit_1_a() { bit(1 << 1, registers.a); }

// 0x50
void bit_2_b() { bit(1 << 2, registers.b); }

// 0x51
void bit_2_c() { bit(1 << 2, registers.c); }

// 0x52
void bit_2_d() { bit(1 << 2, registers.d); }

// 0x53
void bit_2_e() { bit(1 << 2, registers.e); }

// 0x54
void bit_2_h() { bit(1 << 2, registers.h); }

// 0x55
void bit_2_l() { bit(1 << 2, registers.l); }

// 0x56
void bit_2_hlp() { bit(1 << 2, mmu_read_byte(registers.hl)); }

// 0x57
void bit_2_a() { bit(1 << 2, registers.a); }

// 0x58
void bit_3_b() { bit(1 << 3, registers.b); }

// 0x59
void bit_3_c() { bit(1 << 3, registers.c); }

// 0x5a
void bit_3_d() { bit(1 << 3, registers.d); }

// 0x5b
void bit_3_e() { bit(1 << 3, registers.e); }

// 0x5c
void bit_3_h() { bit(1 << 3, registers.h); }

// 0x5d
void bit_3_l() { bit(1 << 3, registers.l); }

// 0x5e
void bit_3_hlp() { bit(1 << 3, mmu_read_byte(registers.hl)); }

// 0x5f
void bit_3_a() { bit(1 << 3, registers.a); }

// 0x60
void bit_4_b() { bit(1 << 4, registers.b); }

// 0x61
void bit_4_c() { bit(1 << 4, registers.c); }

// 0x62
void bit_4_d() { bit(1 << 4, registers.d); }

// 0x63
void bit_4_e() { bit(1 << 4, registers.e); }

// 0x64
void bit_4_h() { bit(1 << 4, registers.h); }

// 0x65
void bit_4_l() { bit(1 << 4, registers.l); }

// 0x66
void bit_4_hlp() { bit(1 << 4, mmu_read_byte(registers.hl)); }

// 0x67
void bit_4_a() { bit(1 << 4, registers.a); }

// 0x68
void bit_5_b() { bit(1 << 5, registers.b); }

// 0x69
void bit_5_c() { bit(1 << 5, registers.c); }

// 0x6a
void bit_5_d() { bit(1 << 5, registers.d); }

// 0x6b
void bit_5_e() { bit(1 << 5, registers.e); }

// 0x6c
void bit_5_h() { bit(1 << 5, registers.h); }

// 0x6d
void bit_5_l() { bit(1 << 5, registers.l); }

// 0x6e
void bit_5_hlp() { bit(1 << 5, mmu_read_byte(registers.hl)); }

// 0x6f
void bit_5_a() { bit(1 << 5, registers.a); }

// 0x70
void bit_6_b() { bit(1 << 6, registers.b); }

// 0x71
void bit_6_c() { bit(1 << 6, registers.c); }

// 0x72
void bit_6_d() { bit(1 << 6, registers.d); }

// 0x73
void bit_6_e() { bit(1 << 6, registers.e); }

// 0x74
void bit_6_h() { bit(1 << 6, registers.h); }

// 0x75
void bit_6_l() { bit(1 << 6, registers.l); }

// 0x76
void bit_6_hlp() { bit(1 << 6, mmu_read_byte(registers.hl)); }

// 0x77
void bit_6_a() { bit(1 << 6, registers.a); }

// 0x78
void bit_7_b() { bit(1 << 7, registers.b); }

// 0x79
void bit_7_c() { bit(1 << 7, registers.c); }

// 0x7a
void bit_7_d() { bit(1 << 7, registers.d); }

// 0x7b
void bit_7_e() { bit(1 << 7, registers.e); }

// 0x7c
void bit_7_h() { bit(1 << 7, registers.h); }

// 0x7d
void bit_7_l() { bit(1 << 7, registers.l); }

// 0x7e
void bit_7_hlp() { bit(1 << 7, mmu_read_byte(registers.hl)); }

// 0x7f
void bit_7_a() { bit(1 << 7, registers.a); }

// 0x80
void res_0_b() { registers.b &= ~(1 << 0); }

// 0x81
void res_0_c() { registers.c &= ~(1 << 0); }

// 0x82
void res_0_d() { registers.d &= ~(1 << 0); }

// 0x83
void res_0_e() { registers.e &= ~(1 << 0); }

// 0x84
void res_0_h() { registers.h &= ~(1 << 0); }

// 0x85
void res_0_l() { registers.l &= ~(1 << 0); }

// 0x86
void res_0_hlp() { mmu_write_byte(registers.hl, mmu_read_byte(registers.hl) & ~(1 << 0)); }

// 0x87
void res_0_a() { registers.a &= ~(1 << 0); }

// 0x88
void res_1_b() { registers.b &= ~(1 << 1); }

// 0x89
void res_1_c() { registers.c &= ~(1 << 1); }

// 0x8a
void res_1_d() { registers.d &= ~(1 << 1); }

// 0x8b
void res_1_e() { registers.e &= ~(1 << 1); }

// 0x8c
void res_1_h() { registers.h &= ~(1 << 1); }

// 0x8d
void res_1_l() { registers.l &= ~(1 << 1); }

// 0x8e
void res_1_hlp() { mmu_write_byte(registers.hl, registers.hl & ~(1 << 1)); }

// 0x8f
void res_1_a() { registers.a &= ~(1 << 1); }

// 0x90
void res_2_b() { registers.b &= ~(1 << 2); }

// 0x91
void res_2_c() { registers.c &= ~(1 << 2); }

// 0x92
void res_2_d() { registers.d &= ~(1 << 2); }

// 0x93
void res_2_e() { registers.e &= ~(1 << 2); }

// 0x94
void res_2_h() { registers.h &= ~(1 << 2); }

// 0x95
void res_2_l() { registers.l &= ~(1 << 2); }

// 0x96
void res_2_hlp() { mmu_write_byte(registers.hl, registers.hl & ~(1 << 2)); }

// 0x97
void res_2_a() { registers.a &= ~(1 << 2); }

// 0x98
void res_3_b() { registers.b &= ~(1 << 3); }

// 0x99
void res_3_c() { registers.c &= ~(1 << 3); }

// 0x9a
void res_3_d() { registers.d &= ~(1 << 3); }

// 0x9b
void res_3_e() { registers.e &= ~(1 << 3); }

// 0x9c
void res_3_h() { registers.h &= ~(1 << 3); }

// 0x9d
void res_3_l() { registers.l &= ~(1 << 3); }

// 0x9e
void res_3_hlp() { mmu_write_byte(registers.hl, registers.hl & ~(1 << 3)); }

// 0x9f
void res_3_a() { registers.a &= ~(1 << 3); }

// 0xa0
void res_4_b() { registers.b &= ~(1 << 4); }

// 0xa1
void res_4_c() { registers.c &= ~(1 << 4); }

// 0xa2
void res_4_d() { registers.d &= ~(1 << 4); }

// 0xa3
void res_4_e() { registers.e &= ~(1 << 4); }

// 0xa4
void res_4_h() { registers.h &= ~(1 << 4); }

// 0xa5
void res_4_l() { registers.l &= ~(1 << 4); }

// 0xa6
void res_4_hlp() { mmu_write_byte(registers.hl, registers.hl & ~(1 << 4)); }

// 0xa7
void res_4_a() { registers.a &= ~(1 << 4); }

// 0xa8
void res_5_b() { registers.b &= ~(1 << 5); }

// 0xa9
void res_5_c() { registers.c &= ~(1 << 5); }

// 0xaa
void res_5_d() { registers.d &= ~(1 << 5); }

// 0xab
void res_5_e() { registers.e &= ~(1 << 5); }

// 0xac
void res_5_h() { registers.h &= ~(1 << 5); }

// 0xad
void res_5_l() { registers.l &= ~(1 << 5); }

// 0xae
void res_5_hlp() { mmu_write_byte(registers.hl, registers.hl & ~(1 << 5)); }

// 0xaf
void res_5_a() { registers.a &= ~(1 << 5); }

// 0xb0
void res_6_b() { registers.b &= ~(1 << 6); }

// 0xb1
void res_6_c() { registers.c &= ~(1 << 6); }

// 0xb2
void res_6_d() { registers.d &= ~(1 << 6); }

// 0xb3
void res_6_e() { registers.e &= ~(1 << 6); }

// 0xb4
void res_6_h() { registers.h &= ~(1 << 6); }

// 0xb5
void res_6_l() { registers.l &= ~(1 << 6); }

// 0xb6
void res_6_hlp() { mmu_write_byte(registers.hl, registers.hl & ~(1 << 6)); }

// 0xb7
void res_6_a() { registers.a &= ~(1 << 6); }

// 0xb8
void res_7_b() { registers.b &= ~(1 << 7); }

// 0xb9
void res_7_c() { registers.c &= ~(1 << 7); }

// 0xba
void res_7_d() { registers.d &= ~(1 << 7); }

// 0xbb
void res_7_e() { registers.e &= ~(1 << 7); }

// 0xbc
void res_7_h() { registers.h &= ~(1 << 7); }

// 0xbd
void res_7_l() { registers.l &= ~(1 << 7); }

// 0xbe
void res_7_hlp() { mmu_write_byte(registers.hl, mmu_read_byte(registers.hl) & ~(1 << 7)); }

// 0xbf
void res_7_a() { registers.a &= ~(1 << 7); }

// 0xc0
void set_0_b() { registers.b = set(1 << 0, registers.b); }

// 0xc1
void set_0_c() { registers.c = set(1 << 0, registers.c); }

// 0xc2
void set_0_d() { registers.d = set(1 << 0, registers.d); }

// 0xc3
void set_0_e() { registers.e = set(1 << 0, registers.e); }

// 0xc4
void set_0_h() { registers.h = set(1 << 0, registers.h); }

// 0xc5
void set_0_l() { registers.l = set(1 << 0, registers.l); }

// 0xc6
void set_0_hlp() { mmu_write_byte(registers.hl, set(1 << 0, mmu_read_byte(registers.hl))); }

// 0xc7
void set_0_a() { registers.a = set(1 << 0, registers.a); }

// 0xc8
void set_1_b() { registers.b = set(1 << 1, registers.b); }

// 0xc9
void set_1_c() { registers.c = set(1 << 1, registers.c); }

// 0xca
void set_1_d() { registers.d = set(1 << 1, registers.d); }

// 0xcb
void set_1_e() { registers.e = set(1 << 1, registers.e); }

// 0xcc
void set_1_h() { registers.h = set(1 << 1, registers.h); }

// 0xcd
void set_1_l() { registers.l = set(1 << 1, registers.l); }

// 0xce
void set_1_hlp() { mmu_write_byte(registers.hl, set(1 << 1, mmu_read_byte(registers.hl))); }

// 0xcf
void set_1_a() { registers.a = set(1 << 1, registers.a); }

// 0xd0
void set_2_b() { registers.b = set(1 << 2, registers.b); }

// 0xd1
void set_2_c() { registers.c = set(1 << 2, registers.c); }

// 0xd2
void set_2_d() { registers.d = set(1 << 2, registers.d); }

// 0xd3
void set_2_e() { registers.e = set(1 << 2, registers.e); }

// 0xd4
void set_2_h() { registers.h = set(1 << 2, registers.h); }

// 0xd5
void set_2_l() { registers.l = set(1 << 2, registers.l); }

// 0xd6
void set_2_hlp() { mmu_write_byte(registers.hl, set(1 << 2, mmu_read_byte(registers.hl))); }

// 0xd7
void set_2_a() { registers.a = set(1 << 2, registers.a); }

// 0xd8
void set_3_b() { registers.b = set(1 << 3, registers.b); }

// 0xd9
void set_3_c() { registers.c = set(1 << 3, registers.c); }

// 0xda
void set_3_d() { registers.d = set(1 << 3, registers.d); }

// 0xdb
void set_3_e() { registers.e = set(1 << 3, registers.e); }

// 0xdc
void set_3_h() { registers.h = set(1 << 3, registers.h); }

// 0xdd
void set_3_l() { registers.l = set(1 << 3, registers.l); }

// 0xde
void set_3_hlp() { mmu_write_byte(registers.hl, set(1 << 3, mmu_read_byte(registers.hl))); }

// 0xdf
void set_3_a() { registers.a = set(1 << 3, registers.a); }

// 0xe0
void set_4_b() { registers.b = set(1 << 4, registers.b); }

// 0xe1
void set_4_c() { registers.c = set(1 << 4, registers.c); }

// 0xe2
void set_4_d() { registers.d = set(1 << 4, registers.d); }

// 0xe3
void set_4_e() { registers.e = set(1 << 4, registers.e); }

// 0xe4
void set_4_h() { registers.h = set(1 << 4, registers.h); }

// 0xe5
void set_4_l() { registers.l = set(1 << 4, registers.l); }

// 0xe6
void set_4_hlp() { mmu_write_byte(registers.hl, set(1 << 4, mmu_read_byte(registers.hl))); }

// 0xe7
void set_4_a() { registers.a = set(1 << 4, registers.a); }

// 0xe8
void set_5_b() { registers.b = set(1 << 5, registers.b); }

// 0xe9
void set_5_c() { registers.c = set(1 << 5, registers.c); }

// 0xea
void set_5_d() { registers.d = set(1 << 5, registers.d); }

// 0xeb
void set_5_e() { registers.e = set(1 << 5, registers.e); }

// 0xec
void set_5_h() { registers.h = set(1 << 5, registers.h); }

// 0xed
void set_5_l() { registers.l = set(1 << 5, registers.l); }

// 0xee
void set_5_hlp() { mmu_write_byte(registers.hl, set(1 << 5, mmu_read_byte(registers.hl))); }

// 0xef
void set_5_a() { registers.a = set(1 << 5, registers.a); }

// 0xf0
void set_6_b() { registers.b = set(1 << 6, registers.b); }

// 0xf1
void set_6_c() { registers.c = set(1 << 6, registers.c); }

// 0xf2
void set_6_d() { registers.d = set(1 << 6, registers.d); }

// 0xf3
void set_6_e() { registers.e = set(1 << 6, registers.e); }

// 0xf4
void set_6_h() { registers.h = set(1 << 6, registers.h); }

// 0xf5
void set_6_l() { registers.l = set(1 << 6, registers.l); }

// 0xf6
void set_6_hlp() { mmu_write_byte(registers.hl, set(1 << 6, mmu_read_byte(registers.hl))); }

// 0xf7
void set_6_a() { registers.a = set(1 << 6, registers.a); }

// 0xf8
void set_7_b() { registers.b = set(1 << 7, registers.b); }

// 0xf9
void set_7_c() { registers.c = set(1 << 7, registers.c); }

// 0xfa
void set_7_d() { registers.d = set(1 << 7, registers.d); }

// 0xfb
void set_7_e() { registers.e = set(1 << 7, registers.e); }

// 0xfc
void set_7_h() { registers.h = set(1 << 7, registers.h); }

// 0xfd
void set_7_l() { registers.l = set(1 << 7, registers.l); }

// 0xfe
void set_7_hlp() { mmu_write_byte(registers.hl, set(1 << 7, mmu_read_byte(registers.hl))); }

// 0xff
void set_7_a() { registers.a = set(1 << 7, registers.a); }