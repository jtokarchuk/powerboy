# PowerBoy
Gameboy/Gameboy Color emulation project. Mostly just a fun way for me to reinvent the wheel on something fun.

## Goals
 - [x] Passes all 11 [Blargg CPU Instruction Tests](https://github.com/retrio/gb-test-roms)
 - [ ] RAM bank support
 - [ ] ROM bank support
 - [ ] Real-Time Debugger
 - [ ] CGB support
 - [ ] Sound support
 - [ ] Catch a pidgey on something I made.
 - [ ] GUI

## Playable Games
- Tetris
- Dr. Mario

## Dependencies
- SDL2

## Known issues
- Rom banking not supported yet, only 32kb games with no mappers.
- Has not been tested / compiled outside of Windows, expect issues.

## Controls
| Keyboard Key  | Gameboy Key   |
| ------------- | ------------- |
| Up Arrow      | UP            |
| Down Arrow    | DOWN          |
| Left Arrow    | Left          |
| Right Arrow   | Right         |
| Z Key         | B             |
| X Key         | A             |
| Return Key    | Start         |
| Spacebar      | Select        |

## References
- [Cinoop](https://github.com/CTurt/Cinoop)
- [Gameboy Doctor](https://robertheaton.com/gameboy-doctor/)
- [Pandocs](https://gbdev.io/pandocs/)