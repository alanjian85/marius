#include "settings.h"
using namespace nes;

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <SDL_keyboard.h>

Settings::Settings(const char* path) {
    loadDefaults();

    std::ifstream file(path);
    if (file.is_open()) {
        nlohmann::json json;
        if (!(file >> json)) {
            throw std::runtime_error("Failed to load settings file");
            return;
        }

        if (auto it = json.find("dump_size"); it != json.cend()) dump_size = *it;

        if (auto it = json.find("keymaps"); it != json.cend()) {
            auto& keymaps = *it;

            if (auto it = keymaps.find("dump"); it != keymaps.cend()) dump = getScancode(*it);
            if (auto it = keymaps.find("reset"); it != keymaps.cend()) reset = getScancode(*it);

            if (auto it = keymaps.find("controller1"); it != keymaps.cend()) {
                auto& map1 = *it;

                if (auto it = map1.find("a"); it != map1.cend())      keymap1.a = getScancode(*it);
                if (auto it = map1.find("b"); it != map1.cend())      keymap1.b = getScancode(*it);
                if (auto it = map1.find("select"); it != map1.cend()) keymap1.select = getScancode(*it);
                if (auto it = map1.find("start"); it != map1.cend())  keymap1.start = getScancode(*it);
                if (auto it = map1.find("up"); it != map1.cend())     keymap1.up = getScancode(*it);
                if (auto it = map1.find("down"); it != map1.cend())   keymap1.down = getScancode(*it);
                if (auto it = map1.find("left"); it != map1.cend())   keymap1.left = getScancode(*it);
                if (auto it = map1.find("right"); it != map1.cend())  keymap1.right = getScancode(*it);
            }

            if (auto it = keymaps.find("controller2"); it != keymaps.cend()) {
                auto& map2 = *it;

                if (auto it = map2.find("a"); it != map2.cend())      keymap2.a = getScancode(*it);
                if (auto it = map2.find("b"); it != map2.cend())      keymap2.b = getScancode(*it);
                if (auto it = map2.find("select"); it != map2.cend()) keymap2.select = getScancode(*it);
                if (auto it = map2.find("start"); it != map2.cend())  keymap2.start = getScancode(*it);
                if (auto it = map2.find("up"); it != map2.cend())     keymap2.up = getScancode(*it);
                if (auto it = map2.find("down"); it != map2.cend())   keymap2.down = getScancode(*it);
                if (auto it = map2.find("left"); it != map2.cend())   keymap2.left = getScancode(*it);
                if (auto it = map2.find("right"); it != map2.cend())  keymap2.right = getScancode(*it);
            }
        }
    }
}

void Settings::loadDefaults() {
    dump_size = 32;

    dump = SDL_SCANCODE_F1;
    reset = SDL_SCANCODE_F2;

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

SDL_Scancode Settings::getScancode(const std::string& name) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(name.c_str());
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        throw std::invalid_argument("Scancode name " + name + " is invalid");
    }
    return scancode;
}