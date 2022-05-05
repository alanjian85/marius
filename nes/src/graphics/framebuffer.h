#ifndef NES_IO_FRAMEBUFFER_H_
#define NES_IO_FRAMEBUFFER_H_

#include <cstdint>
#include <vector>

#include <SDL.h>

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

        void setPixel(int x, int y, std::uint32_t color);

        void update();
    private:
        int width_;
        int height_;
        SDL_Texture* texture_;
        std::vector<std::uint32_t> pixels_;
    };
}

#endif // NES_IO_FRAMEBUFFER_H_