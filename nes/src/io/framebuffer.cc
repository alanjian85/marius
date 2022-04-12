#include "framebuffer.h"
using namespace nes;

Framebuffer::Framebuffer(int width, int height)
    : pixels_(width * height)
{
    width_ = width;
    height_ = height;
}

void Framebuffer::setPixel(int x, int y, std::uint32_t color) {
    pixels_[y * width_ + x] = color;
}

const void* Framebuffer::getPixels() const {
    return pixels_.data();
}