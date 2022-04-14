#ifndef NES_IO_FRAMEBUFFER_H_
#define NES_IO_FRAMEBUFFER_H_

#include <cstdint>
#include <vector>

namespace nes {
    class Framebuffer final {
    public:
        Framebuffer(int width, int height);

        void setPixel(int x, int y, std::uint32_t color);

        [[nodiscard]] const void* getPixels() const;
    private:
        int width_, height_;
        std::vector<std::uint32_t> pixels_;
    };
}

#endif // NES_IO_FRAMEBUFFER_H_