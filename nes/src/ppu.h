#ifndef NES_PPU_H_
#define NES_PPU_H_

#include <SDL2/SDL.h>

namespace nes {
    class Ppu {
    public:
        Ppu();

        Ppu(const Ppu&) = delete;

        Ppu& operator=(const Ppu&) = delete;

        ~Ppu();
    private:
        SDL_Texture* texture_;
    };
}

#endif // NES_PPU_H_