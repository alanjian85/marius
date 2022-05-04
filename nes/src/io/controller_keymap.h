#ifndef NES_KEYMAP_H_
#define NES_KEYMAP_H_

#include <SDL2/SDL.h>

namespace nes {
    struct ControllerKeymap final {
        SDL_Scancode a;
        SDL_Scancode b;
        SDL_Scancode select;
        SDL_Scancode start;
        SDL_Scancode up;
        SDL_Scancode down;
        SDL_Scancode left;
        SDL_Scancode right;
    };
}

#endif // NES_KEYMAP_H_