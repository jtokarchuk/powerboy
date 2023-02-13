#pragma once
#include <stdio.h>

#include "platform.h"

#define INTERRUPTS_VBLANK       (1 << 0)
#define INTERRUPTS_LCDSTAT      (1 << 1)
#define INTERRUPTS_TIMER        (1 << 2)
#define INTERRUPTS_SERIAL       (1 << 3)
#define INTERRUPTS_JOYPAD       (1 << 4)

struct interrupt {
    unsigned char master;
    unsigned char enable;
    unsigned char flags;
} extern interrupt;

extern void interrupts_emulate();
extern void interrupts_fire_joypad();
void interrupts_vblank();
void interrupts_lcd_stat();
void interrupts_timer();
void interrupts_serial();
void interrupts_joypad();

void interrupts_return();