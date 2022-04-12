#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <cstdint>
#include <vector>

#include <SDL2/SDL.h>

namespace nes {
    class Framebuffer final {
    public:
        Framebuffer(int width, int height);

        void setPixel(int x, int y, std::uint32_t color);

        [[nodiscard]] const std::uint32_t* getPixels() const;
    private:
        int width_, height_;
        std::vector<std::uint32_t> pixels_;
    };
}

#endif // FRAMEBUFFER_H_