#ifndef NES_SETTINGS_H_
#define NES_SETTINGS_H_

#include <string>

#include "io/controller_keymap.h"

namespace nes {
    struct Settings {
        Settings(const char* path);

        void loadDefaults();

        SDL_Scancode getScancode(const std::string& name);

        int dump_size;
        SDL_Scancode dump;
        SDL_Scancode reset;
        ControllerKeymap keymap1, keymap2;
    };
}

#endif // NES_SETTINGS_H_