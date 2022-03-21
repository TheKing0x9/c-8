# Chip8 emulator

A chip8 emulator written with Raylib and Raygui in C.

## Features

C-8 supports:

- Loading and running chip-8 roms
- Customizable execution speed of ROMS
- Ability to pause the execution and step through the instructions one at a time
- Register visualizer to visualize the register state
- Stack visualizer, with the current active element displayed.
- CPU visualizer, to see the instructions. Also includes support for following the currently executing instruction.
- Several settings to resolve Several chip-8 ambiguities over the years.
- Custom themes, via Raygui themes.

## Limitations

The project, though complete has some limitations, which will be fixed soon.

- Don't use `printf("%c", 7)`. Instead, the builtin raylib audio features should be used.

## Credits

This project would not have been possible without:
- `Raylib` and `Raygui` by raysan5.
- `How to create your very own chip-8 emulator` blog post on [Freecodecamp](https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/).
- `Guide to making a chip-8 emulator` by Tobias V. Langhoff on [his blog](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).
