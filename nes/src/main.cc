#include <cassert>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "emulator.h"
#include "io/keymap.h"
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

    Keymap keymap1;
    Keymap keymap2;

    std::ifstream keymap_file("keymap.json");
    if (keymap_file.is_open()) {
        nlohmann::json json;
        keymap_file >> json;

        keymap1.a = GetScancode(json[0]["A"]);
        keymap1.b = GetScancode(json[0]["B"]);
        keymap1.select = GetScancode(json[0]["Select"]);
        keymap1.start = GetScancode(json[0]["Start"]);
        keymap1.up = GetScancode(json[0]["Up"]);
        keymap1.down = GetScancode(json[0]["Down"]);
        keymap1.left = GetScancode(json[0]["Left"]);
        keymap1.right = GetScancode(json[0]["Right"]);

        keymap2.a = GetScancode(json[1]["A"]);
        keymap2.b = GetScancode(json[1]["B"]);
        keymap2.select = GetScancode(json[1]["Select"]);
        keymap2.start = GetScancode(json[1]["Start"]);
        keymap2.up = GetScancode(json[1]["Up"]);
        keymap2.down = GetScancode(json[1]["Down"]);
        keymap2.left = GetScancode(json[1]["Left"]);
        keymap2.right = GetScancode(json[1]["Right"]);
    } else {
        keymap1.a = SDL_SCANCODE_J;
        keymap1.b = SDL_SCANCODE_K;
        keymap1.select = SDL_SCANCODE_RSHIFT;
        keymap1.start = SDL_SCANCODE_RETURN;
        keymap1.up = SDL_SCANCODE_W; 
        keymap1.down = SDL_SCANCODE_S;
        keymap1.left = SDL_SCANCODE_A;
        keymap1.right = SDL_SCANCODE_D;

        keymap2.a = SDL_SCANCODE_X;
        keymap2.b = SDL_SCANCODE_Z;
        keymap2.select = SDL_SCANCODE_LSHIFT;
        keymap2.start = SDL_SCANCODE_TAB;
        keymap2.up = SDL_SCANCODE_UP;
        keymap2.down = SDL_SCANCODE_DOWN;
        keymap2.left = SDL_SCANCODE_LEFT; 
        keymap2.right = SDL_SCANCODE_RIGHT;
    }

    Emulator emulator(argv[1], keymap1, keymap2);
    emulator.run();

    return EXIT_SUCCESS;
}