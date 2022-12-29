#include "framebuffer.h"
using namespace nes;

#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>
#include <string>

Framebuffer::Framebuffer() : texture_(nullptr) {}

Framebuffer::Framebuffer(const Renderer& renderer, int width, int height)
    : pixels_(width * height) {
    width_ = width;
    height_ = height;
    texture_ = SDL_CreateTexture(renderer.getHandle(), SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_STREAMING, width, height);

    if (!texture_) {
        throw std::runtime_error("Failed to create framebuffer texture: " +
                                 std::string(SDL_GetError()));
    } else {
        spdlog::info("Framebuffer texture created");
    }
}

Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept
    : pixels_(std::move(rhs.pixels_)) {
    width_ = rhs.width_;
    height_ = rhs.height_;
    texture_ = std::exchange(rhs.texture_, nullptr);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        spdlog::info("Framebuffer texture destroyed");
    }
    width_ = rhs.width_;
    height_ = rhs.height_;
    texture_ = std::exchange(rhs.texture_, nullptr);
    pixels_ = std::move(rhs.pixels_);
    return *this;
}

Framebuffer::~Framebuffer() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        spdlog::info("Framebuffer texture destroyed");
    }
}

SDL_Texture* Framebuffer::getTexture() const { return texture_; }

void Framebuffer::setPixel(int x, int y, std::uint32_t color) {
    assert(x >= 0 && x < width_);
    assert(y >= 0 && y < height_);
    pixels_[y * width_ + x] = color;
}

void Framebuffer::update() {
    if (SDL_UpdateTexture(texture_, nullptr, pixels_.data(),
                          sizeof(std::uint32_t) * width_) < 0) {
        spdlog::critical("Failed to update framebuffer texture: {}",
                         SDL_GetError());
    }
}