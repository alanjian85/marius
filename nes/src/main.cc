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
    try {
        if (argc != 2) {
            std::cerr << "Usage: nes <ROM>";
            return EXIT_FAILURE;
        }

        spdlog::info("ROM Path: {}", argv[1]);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
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
    } catch (std::exception& e) {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}