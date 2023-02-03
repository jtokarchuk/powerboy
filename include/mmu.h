#pragma once
#include <stdlib.h>

struct mmu {
    const unsigned char bios[0x100];
    unsigned char cartridge[0x8000];
    unsigned char sram[0x2000];
    unsigned char io[0x100];
    unsigned char vram[0x2000];
    unsigned char oam[0x100];
    unsigned char wram[0x2000];
    unsigned char hram[0x80];
} extern mmu;

void mmu_copy(unsigned short destination, unsigned short source, size_t length);

unsigned char mmu_read_byte(unsigned short address);
unsigned short mmu_read_short(unsigned short address);
unsigned short mmu_read_short_from_stack(void);

void mmu_write_byte(unsigned short address, unsigned char value);
void mmu_write_short(unsigned short address, unsigned short value);
void mmu_write_short_to_stack(unsigned short value);

