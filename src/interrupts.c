#include "interrupts.h"
#include "gpu.h"
#include "mmu.h"
#include "registers.h"
#include "cpu.h"

struct interrupt interrupt;

void interrupts_emulate() {
	if(interrupt.master && interrupt.enable && interrupt.flags) {
		unsigned char fire = interrupt.enable & interrupt.flags;
		
		if(fire & INTERRUPTS_VBLANK) {
			interrupt.flags &= ~INTERRUPTS_VBLANK;
			interrupts_vblank();
		}
		
		if(fire & INTERRUPTS_LCDSTAT) {
			interrupt.flags &= ~INTERRUPTS_LCDSTAT;
			interrupts_lcd_stat();
		}
		
		if(fire & INTERRUPTS_TIMER) {
			interrupt.flags &= ~INTERRUPTS_TIMER;
			interrupts_timer();
		}
		
		if(fire & INTERRUPTS_SERIAL) {
			interrupt.flags &= ~INTERRUPTS_SERIAL;
			interrupts_serial();
		}
		
		if(fire & INTERRUPTS_JOYPAD) {			
			interrupt.flags &= ~INTERRUPTS_JOYPAD;
			interrupts_joypad();
		}
	}
}

void interrupts_vblank() {
    gpu_draw_framebuffer();

    interrupt.master = 0;
    mmu_write_short_to_stack(registers.pc);
    registers.pc = 0x40;
    cpu.ticks += 12;
}

void interrupts_lcd_stat() {
    interrupt.master = 0;
    mmu_write_short_to_stack(registers.pc);
    registers.pc = 0x48;
    cpu.ticks += 12;
}

void interrupts_timer() {
	interrupt.master = 0;
	mmu_write_short_to_stack(registers.pc);
	registers.pc = 0x50;
	cpu.ticks += 12;
}

void interrupts_serial() {
    interrupt.master = 0;
    mmu_write_short_to_stack(registers.pc);
    registers.pc = 0x58;
    cpu.ticks += 12;
}

void interrupts_joypad() {
    interrupt.master = 0;
    mmu_write_short_to_stack(registers.pc);
    registers.pc = 0x60;
    cpu.ticks += 12;
}

void interrupts_return() {
    interrupt.master = 1;
    registers.pc = mmu_read_short_from_stack();
}
