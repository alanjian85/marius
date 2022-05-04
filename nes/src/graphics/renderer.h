#ifndef NES_GRAPHICS_RENDERER_H_
#define NES_GRAPHICS_RENDERER_H_

#include <SDL2/SDL.h>

#include "window.h"

namespace nes {
    class Renderer final {
    public:
        Renderer();

        Renderer(const Window& window, int index, Uint32 flags);

        Renderer(Renderer&& rhs) noexcept;

        Renderer& operator=(Renderer&& rhs) noexcept;

        ~Renderer();

        [[nodiscard]] SDL_Renderer* getHandle() const;

        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

        void clear();

        void copy(SDL_Texture* texture, SDL_Rect* src_rect, SDL_Rect* dst_rect);

        void present();
    private:
        SDL_Renderer* handle_;
    };
}

#endif // NES_GRAPHICS_RENDERER_H_