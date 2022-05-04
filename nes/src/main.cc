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

    spdlog::info("ROM Path: {}", argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return EXIT_FAILURE;
    } else {
        spdlog::info("SDL initialized");
    }

    {
        Settings settings("settings.json");
        Emulator emulator(argv[1], settings);
        emulator.run();
    }

    SDL_Quit();
    spdlog::info("SDL quit");
    return EXIT_SUCCESS;
}