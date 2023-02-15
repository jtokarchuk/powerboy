#pragma once

#include "platform.h"

void timer_emulate(int cycles);
void timer_reset();

extern int timer_frequency;
extern int timer_counter;
extern int timer_dividercounter;

