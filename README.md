# Chip-8 Emulator
A Chip-8 "Emulator" (technically interpreter) written in C using Raylib.

### Build
Compile [raylib](https://github.com/raysan5/raylib/wiki) as a static library and put the compiled libraylib.a in the "lib" folder.

Then type `make release` to compile and run the executable.

### Controls
The Keyboard for Chip-8 Systems looked like this:

<img src="https://github.com/AnimePenguin/Chip8-Emulator/assets/78182394/232ca4ef-f381-46c6-9b7f-a1ad5b946b0c" height=250>

They are mapped to these keys on your keyboard.
```
Keyboard    Chip-8
1 2 3 4     1 2 3 C
Q W E R  -> 4 5 6 D
A S D F     7 8 9 E
Z X C V     A 0 B F
```

Most Games use 2, Q, E, S for movement and W as an extra action.

#### Emulator Keybindings
- `SPACE` key to pause the emulator.
- `P` key to restart the game.
- `O` key to open file dialog and select a new rom.
- `;` key to open the debugger.
- `L` key to run the game at a slower clock speed (for debugging).

### Compatibility
This emulator should work with most of the Chip-8 roms found on the internet.
Quirks for 8XY6/8XYE and FX55/FX65 are not implemented.

Some example games are provided in the "roms" folder.

### Resources
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
- https://en.wikipedia.org/wiki/CHIP-8
