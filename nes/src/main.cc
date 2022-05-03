#include <cassert>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "emulator.h"
#include "settings.h"
using namespace nes;

SDL_Scancode GetScancode(const std::string& name) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(name.c_str());
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        throw std::runtime_error(std::string("Error: Scancode name ") + name + " is invalid");
    }
    return scancode;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: nes <ROM>";
        return EXIT_FAILURE;
    }

    Settings settings("settings.json");
    Emulator emulator(argv[1], settings);
    emulator.run();

    return EXIT_SUCCESS;
}