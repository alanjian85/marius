#include "framebuffer.h"
using namespace nes;

Framebuffer::Framebuffer(SDL_Renderer* renderer, int width, int height) {
    texture_ = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
}

Framebuffer::~Framebuffer() {
    SDL_DestroyTexture(texture_);
}

std::uint32_t* Framebuffer::lock(int& pitch) {
    void* pixels;
    SDL_LockTexture(texture_, nullptr, &pixels, &pitch);
    return static_cast<std::uint32_t*>(pixels);
}

void Framebuffer::unlock() {
    SDL_UnlockTexture(texture_);
}

SDL_Texture* Framebuffer::getTexture() const {
    return texture_;
}