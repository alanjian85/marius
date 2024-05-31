# Marius
Marius (/ˈma.ri.us/, the Latin root of the Italian name "Mario") is a tiny emulator of NES (Nintendo Entertainment System). It illustrates the fundamental workings of NES by emulating its core components through straightforward algorithms. It also supports various configuration options, debug utilities and a range of common catridge mappers. Moreover, it can be run directly in modern browsers thanks to the powerful WebAssembly technology.

![Mario GIF](mario.gif) ![Pacman GIF](pacman.gif) ![Donkey Kong GIF](donkeykong.gif)

### Key Mappings
| Button | Controller 1 | Controller 2 |
|--------|--------------|--------------|
|   A    |      J       |      X       |
|   B    |      K       |      Z       |
| SELECT |    RSHIFT    |    LSHIFT    |
| START  |    ENTER     |     TAB      |
|   ←    |      A       |      ←       |
|   ↑    |      w       |      ↑       |
|   →    |      D       |      →       |
|   ↓    |      S       |      ↓       |

The log of executed 6502 instructions is dumped when F1 is pressed, and the game console states are reset upon pressing F2.

## Build Instructions
To build Marius, the following libraries are required (you can install them using the package manager of your operating system):
* [SDL2](https://www.libsdl.org/)
* [JSON for Modern C++](https://json.nlohmann.me/)
* [spdlog](https://github.com/gabime/spdlog)

After the dependencies listed above are all installed, run the following instructions to build Marius:
```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The compiled binary can then be found in the directory `build/marius`. The path to the ROM file that is to be emulated should be provided as the first command line argument.

## References
* [NESdev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
* [An Overview of NES Rendering](https://austinmorlan.com/posts/nes_rendering_overview/)
