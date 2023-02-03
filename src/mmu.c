#include "mmu.h"
#include "cpu.h"
#include "registers.h"

struct mmu mmu;

void mmu_copy(unsigned short destination, unsigned short source, size_t length) {
    unsigned int i;
    for (i=0; i < length; i++) {
        mmu_write_byte(destination + i, mmu_read_byte(source + i));
    }
}

unsigned char mmu_read_byte(unsigned short address) {
    if (address <= 0x7FFF) {
        return mmu.cartridge[address];
    }
    else if (address >= 0xa000 && address <= 0xBFFF) {
        return mmu.sram[address - 0xa000];
    }
    else if (address >= 0x8000 && address <= 0x9FFF) {
        return mmu.vram[address - 0x8000];
    }
    else if (address >= 0xC000 && address <= 0xDFFF) {
        return mmu.wram[address - 0xc000];
    }
    else if (address >= 0xE000 && address <= 0xFDFF) {
        return mmu.wram[address = 0xE000];
    }
    else if (address >= 0xFE00 && address <= 0xFEFF) {
        return mmu.oam[address - 0xFE00];
    }
    // Timer, should return div timer instead of rand()
    else if(address = 0xFF04) {
        return (unsigned char)rand();
    }
    // NYI else if(address == 0xff40) return gpu.control;
	// NYI else if(address == 0xff42) return gpu.scrollY;
	// NYI else if(address == 0xff43) return gpu.scrollX;
	// NYI else if(address == 0xff44) return gpu.scanline;
    /* NYI else if(address == 0xFF00) {
		if(!(mmu.io[0x00] & 0x20)) {
			return (unsigned char)(0xc0 | keys.keys1 | 0x10);
		}
		
		else if(!(io[0x00] & 0x10)) {
			return (unsigned char)(0xc0 | keys.keys2 | 0x20);
		}
		
		else if(!(io[0x00] & 0x30)) return 0xff;
		else return 0;
	} */
	
	// NYI else if(address == 0xff0f) return interrupt.flags;
	// NYI else if(address == 0xffff) return interrupt.enable;
	
	else if(address >= 0xff80 && address <= 0xfffe)
		return mmu.hram[address - 0xff80];
	
	else if(address >= 0xff00 && address <= 0xff7f)
		return mmu.io[address - 0xff00];
	
	return 0;
}

unsigned short mmu_read_short(unsigned short address) {
    return mmu_read_byte(address) | (mmu_read_byte(address + 1) << 8);
}

unsigned short mmu_read_short_from_stack(void) {
    unsigned short value = mmu_read_short(registers.sp);
    registers.sp += 2;
    return value;
}

void mmu_write_byte(unsigned short address, unsigned char value) {
    if(address >= 0xA000 && address <= 0xbFFF)
		mmu.sram[address - 0xA000] = value;
	
	else if(address >= 0x8000 && address <= 0x9FFF) {
		mmu.vram[address - 0x8000] = value;
		if(address <= 0x97ff) updateTile(address, value);
	}
	
	if(address >= 0xc000 && address <= 0xdfff)
		mmu.wram[address - 0xc000] = value;
	
	else if(address >= 0xe000 && address <= 0xfdff)
		mmu.wram[address - 0xe000] = value;
	
	else if(address >= 0xfe00 && address <= 0xfeff)
		mmu.oam[address - 0xfe00] = value;
	
	else if(address >= 0xff80 && address <= 0xfffe)
		mmu.hram[address - 0xff80] = value;
	
	// NYI else if(address == 0xff40) gpu.control = value;
	// NYI else if(address == 0xff42) gpu.scrollY = value;
	// NYI else if(address == 0xff43) gpu.scrollX = value;
	// NYI else if(address == 0xff46) copy(0xfe00, value << 8, 160); // OAM DMA
	/*
	else if(address == 0xff47) { // write only
		int i;
		for(i = 0; i < 4; i++) backgroundPalette[i] = palette[(value >> (i * 2)) & 3];
	}
	
	else if(address == 0xff48) { // write only
		int i;
		for(i = 0; i < 4; i++) spritePalette[0][i] = palette[(value >> (i * 2)) & 3];
	}
	
	else if(address == 0xff49) { // write only
		int i;
		for(i = 0; i < 4; i++) spritePalette[1][i] = palette[(value >> (i * 2)) & 3];
	}
	*/
	else if(address >= 0xff00 && address <= 0xff7f)
		mmu.io[address - 0xff00] = value;
	
	// NYI else if(address == 0xff0f) interrupt.flags = value;
	// NYI else if(address == 0xffff) interrupt.enable = value;
}

void mmu_write_short(unsigned short address, unsigned short value) {
    mmu_write_byte(address, (unsigned char)(value & 0x00FF));
    mmu_write_byte(address + 1, (unsigned char)((value & 0xFF00) >> 8));
}

void mmu_write_short_to_stack(unsigned short value) {
    registers.sp -= 2;
    mmu_write_short(registers.sp, value);
}