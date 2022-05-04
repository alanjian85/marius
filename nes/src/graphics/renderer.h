#ifndef NES_GRAPHICS_RENDERER_H_
#define NES_GRAPHICS_RENDERER_H_

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "window.h"

namespace nes {
    class Renderer {
    public:
        Renderer() : handle_(nullptr) {}

        Renderer(const Window& window, int index, Uint32 flags) {
            handle_ = SDL_CreateRenderer(window.getHandle(), index, flags);

            if (!handle_) {
                spdlog::error("Failed to create renderer: {}", SDL_GetError());
            }
        }

        Renderer(Renderer&& rhs) noexcept {
            handle_ = std::exchange(rhs.handle_, nullptr);
        }

        Renderer& operator=(Renderer&& rhs) noexcept {
            if (handle_) {
                SDL_DestroyRenderer(handle_);
                spdlog::info("Renderer destroyed");
            }
            handle_ = std::exchange(rhs.handle_, nullptr);
            return *this;
        }

        ~Renderer() {
            if (handle_) {
                SDL_DestroyRenderer(handle_);
                spdlog::info("Renderer destroyed");
            }
        }

        [[nodiscard]] SDL_Renderer* getHandle() const {
            return handle_;
        }

        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
            SDL_SetRenderDrawColor(handle_, r, g, b, a);
        }

        void clear() {
            SDL_RenderClear(handle_);
        }

        void copy(SDL_Texture* texture, SDL_Rect* src_rect, SDL_Rect* dst_rect) {
            SDL_RenderCopy(handle_, texture, src_rect, dst_rect);
        }

        void present() {
            SDL_RenderPresent(handle_);
        }
    private:
        SDL_Renderer* handle_;
    };
}

#endif // NES_GRAPHICS_RENDERER_H_