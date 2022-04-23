#include <cassert>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

#include "emulator.h"
#include "io/keymap.h"
using namespace nes;

SDL_Scancode GetScancode(const std::string& name) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(name.c_str());
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        throw std::runtime_error(std::string("Error: ") + SDL_GetError());
    }
    return scancode;
}

int main(int argc, char** argv) {
    try {
        if (argc != 2) {
            throw std::invalid_argument("Usage: nes <ROM>");
        }

        std::ifstream rom(argv[1], std::ios::binary);
        if (!rom.is_open()) {
            throw std::runtime_error(std::string("Error: Couldn't open ROM file ") + argv[1]);
        }

        Keymap keymap1;
        Keymap keymap2;

        std::ifstream keymap_file("keymap.json");
        if (keymap_file.is_open()) {
            nlohmann::json json;
            keymap_file >> json;

            keymap1.a = json[0]["A"];
            keymap1.b = json[0]["B"];
            keymap1.select = json[0]["Select"];
            keymap1.start = json[0]["Start"];
            keymap1.up = json[0]["Up"];
            keymap1.down = json[0]["Down"];
            keymap1.left = json[0]["Left"];
            keymap1.right = json[0]["Right"];

            keymap1.a = json[1]["A"];
            keymap1.b = json[1]["B"];
            keymap1.select = json[1]["Select"];
            keymap1.start = json[1]["Start"];
            keymap1.up = json[1]["Up"];
            keymap1.down = json[1]["Down"];
            keymap1.left = json[1]["Left"];
            keymap1.right = json[1]["Right"];
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

        Emulator emulator;
        emulator.run(rom);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}