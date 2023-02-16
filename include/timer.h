#pragma once

#include "platform.h"

void timer_emulate(int cycles);
void timer_reset();

extern unsigned int timer_frequency;
extern unsigned int timer_counter;
extern unsigned int timer_dividercounter;

