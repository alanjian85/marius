#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "emulator.h"
using namespace nes;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: nes <ROM>\n";
        return EXIT_FAILURE;
    }

    std::ifstream rom(argv[1], std::ios::binary);
    if (!rom.is_open()) {
        std::cerr << "Error: Couldn't open ROM file " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    Emulator emulator;
    emulator.run(rom);

    return EXIT_SUCCESS;
}