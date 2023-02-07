#include "mmu.h"
#include "cpu.h"
#include "registers.h"

struct mmu mmu;

const unsigned char mmu_io_reset[0x100] = {
	0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
	0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
	0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00, 0xFE, 0xFF, 0xFF, 0xFF,
	0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
	0x45, 0xEC, 0x52, 0xFA, 0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
	0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE, 0x46, 0x97, 0x33, 0x5E,
	0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74, 0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5,
	0xD0, 0x7A, 0x00, 0x9E, 0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
	0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05, 0xF9, 0x00, 0x0B, 0x00
};

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
    else if(address == 0xFF04) {
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
		//if(address <= 0x97ff) updateTile(address, value);
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