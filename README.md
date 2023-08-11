# NES
NES (NES Emulation Software) is a simple and a proof-of-concept emulator of Nintendo Entertainment System. Its single-threaded and interpreter-based design makes it a emulator for demonstration. The underlying mechanisms are implemented using C++17 and SDL2. And It can now run on the web because of WASM.

![Mario GIF](media/mario.gif) ![Pacman GIF](media/pacman.gif) ![Donkey Kong GIF](media/donkeykong.gif)

## Features
* A simple iNES ROM loader included
* Integrated emulation of the 6502 CPU
* Partial support of PPU (Picture Processing Unit)
* Customizable mappings between the keyboard and the virtual controller
* Tweakable settings in the JSON config file
* Object-oriented wrappers of the hardwares

## Default Key Mappings

### Controller 1
| Controller | Keyboard |
|------------|----------|
|     A      |    J     |
|     B      |    K     |
|   SELECT   |  RSHIFT  |
|   START    |  ENTER   |
|     ←      |    A     |
|     ↑      |    w     |
|     →      |    D     |
|     ↓      |    S     |

### Controller 2
| Controller | Keyboard |
|------------|----------|
|     A      |    X     |
|     B      |    Z     |
|   SELECT   |  LSHIFT  |
|   START    |   TAB    |
|     ←      |    ←     |
|     ↑      |    ↑     |
|     →      |    →     |
|     ↓      |    ↓     |

### Other Keys
| Keyboard | Function |
|----------|----------|
|    F1    | Dump log |
|    F2    | Reset    |

## Supported Mappers
* [NROM](https://www.nesdev.org/wiki/NROM)

## Dependencies
* [SDL2](https://www.libsdl.org/)
* [JSON for Modern C++](https://json.nlohmann.me/)
* [spdlog](https://github.com/gabime/spdlog)

## References
* [NESdev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
* [An Overview of NES Rendering](https://austinmorlan.com/posts/nes_rendering_overview/)

[License (MIT License)](LICENSE)
--------------------------------

<a href="http://opensource.org/licenses/MIT" target="_blank">
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">
</a>

```
Copyright (c) 2022 Alan Jian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
