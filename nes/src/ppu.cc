#include "ppu.h"
using namespace nes;

#include <algorithm>

Ppu::Ppu(Framebuffer& framebuffer)
    : framebuffer_(framebuffer)
{
    cycles_ = 0;
    vblank_ = true;
}

void Ppu::reset() {

}

void Ppu::cycle() {
    if (cycles_-- <= 0) {
        vblank_ = true;
    }
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = vblank_ << 7;
    vblank_ = false;
    return status;
}