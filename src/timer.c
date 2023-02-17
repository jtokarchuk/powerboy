#include <stdbool.h>

#include "timer.h"
#include "mmu.h"
#include "interrupts.h"
#include "cpu.h"

unsigned int timer_counter = 0;
unsigned int timer_dividercounter = 0;
unsigned int timer_frequency = 0;

void timer_reset() {
    timer_counter = 0;
    timer_dividercounter = 0;
    timer_frequency = 1024;
}

void timer_emulate(int cycles) {
    unsigned char timer_attributes = mmu_read_byte(0xFF07);
    timer_dividercounter += cycles;

    if (CHECK_BIT(timer_attributes, 2)) {
        
        timer_counter += cycles * 4;

        if (timer_counter >= timer_frequency) {
            
            bool overflow = false;

            if (mmu_read_byte(0xFF05) == 0xFF) {
                overflow = true;
            }

             mmu.io[0xFF05 - 0xFF00]++; // have to increment this directly as a write through mmu resets it

            if (overflow) {
                interrupt.flags |= INTERRUPTS_TIMER;
                mmu.io[0xFF05 - 0xFF00] = mmu_read_byte(0xFF06);
            }
            
            timer_counter -= timer_frequency;
        }
    }

    // Divider register
    if (timer_dividercounter >= 256) {
        timer_dividercounter -= 256;
        mmu.io[0xFF04 - 0xFF00]++;
    }

}