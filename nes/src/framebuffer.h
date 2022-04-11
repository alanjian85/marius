#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <cstdint>

#include <SDL2/SDL.h>

namespace nes {
    class Framebuffer final {
    public:
        Framebuffer(SDL_Renderer* renderer, int width, int height);

        Framebuffer(const Framebuffer&) = delete;

        Framebuffer& operator=(const Framebuffer&) = delete;

        ~Framebuffer();

        [[nodiscard]] std::uint32_t* lock(int& pitch);

        void unlock();

        [[nodiscard]] SDL_Texture* getTexture() const;
    private:
        SDL_Texture* texture_;
    };
}

#endif // FRAMEBUFFER_H_