#ifndef NES_IO_FRAMEBUFFER_H_
#define NES_IO_FRAMEBUFFER_H_

#include <cstdint>

#include <SDL2/SDL.h>

namespace nes {
    class Framebuffer final {
    public:
        Framebuffer(SDL_Renderer* renderer, int width, int height);

        Framebuffer(const Framebuffer&) = delete;

        Framebuffer& operator=(const Framebuffer&) = delete;

        ~Framebuffer();

        void destroy();

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