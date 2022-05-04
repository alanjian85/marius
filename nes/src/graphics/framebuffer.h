#ifndef NES_IO_FRAMEBUFFER_H_
#define NES_IO_FRAMEBUFFER_H_

#include <cstdint>

#include <SDL2/SDL.h>

#include "renderer.h"

namespace nes {
    class Framebuffer final {
    public:
        Framebuffer();

        Framebuffer(const Renderer& renderer, int width, int height);

        Framebuffer(Framebuffer&& rhs) noexcept;

        Framebuffer& operator=(Framebuffer&& rhs) noexcept;

        ~Framebuffer();

        [[nodiscard]] SDL_Texture* getTexture() const;

        void lock();

        void unlock();

        void setPixel(int x, int y, std::uint32_t color);

        [[nodiscard]] bool isLocked() const;
    private:
        SDL_Texture* texture_;
        void* pixels_;
        int pitch_;
    };
}

#endif // NES_IO_FRAMEBUFFER_H_