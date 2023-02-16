# PowerBoy
gameboy/gameboy color emulation project

## References
Cinoop - https://github.com/CTurt/Cinoop
Gameboy Doctor - https://robertheaton.com/gameboy-doctor/
https://github.com/mpostaire/gbmulator/blob/master/src/emulator/cpu.c

# Blargg CPU tests
 - Passes all 11

# Known issues
- Dr Mario garbled (look at LCDC Setting to see where we should be getting BG and tile data from, sprites not affected)
- Rom banking not enabled
