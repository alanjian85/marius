#ifndef NES_PPU_H_
#define NES_PPU_H_

#include <cstdint>

#include <SDL2/SDL.h>

namespace nes {
    class Ppu {
    public:
        Ppu();

        Ppu(SDL_Renderer* renderer);

        Ppu(const Ppu&) = delete;

        Ppu& operator=(const Ppu&) = delete;

        ~Ppu();

        [[nodiscard]] SDL_Texture* getTexture() const;

        [[nodiscard]] float getAspect() const;

        [[nodiscard]] std::uint8_t getStatus();

        void update();
    private:
        static constexpr int kWidth = 256, kHeight = 240;

        SDL_Texture* texture_;

        bool vblank_;
    };
}

#endif // NES_PPU_H_