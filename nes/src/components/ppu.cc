#include "ppu.h"
using namespace nes;

#include <algorithm>
#include <cassert>

#include "cpu.h"

Ppu::Ppu(Framebuffer& framebuffer, PpuBus& ppu_bus)
    : framebuffer_(framebuffer),
      ppu_bus_(ppu_bus)
{
    cycle_ = 0;
    scanline_ = 261;

    addr_inc_ = 1;
    vblank_nmi_ = false;
    addr_ = 0x00;

    vblank_ = true;
}

void Ppu::reset() {
    cycle_ = 0;
    scanline_ = 261;

    addr_inc_ = 1;
    vblank_nmi_ = false;
}

void Ppu::cycle() {
    if (cycle_ == 0 && scanline_ < 240) {
        for (int x = 0; x < 256; ++x) {
            std::uint8_t tile = ppu_bus_.read(0x2000 + scanline_ / 8 * 32 + x / 8);
            std::uint8_t pattern1 = ppu_bus_.read(0x1000 + tile * 16 + scanline_ % 8);
            std::uint8_t pattern2 = ppu_bus_.read(0x1000 + tile * 16 + 8 + scanline_ % 8);
            std::uint8_t pattern = pattern1 | pattern2;
            if (pattern & 0x80 >> x % 8) {
                framebuffer_.setPixel(x, scanline_, 0xFF0000FF);
            } else {
                framebuffer_.setPixel(x, scanline_, 0x000000FF);
            }
        }
    }

    if (scanline_ == 241 && cycle_ == 0) {
        vblank_ = true;
        if (vblank_nmi_) {
            assert(cpu_);
            cpu_->nmi();
        }
    }

    ++cycle_;
    if (cycle_ == 341) {
        cycle_ = 0;
        scanline_ = (scanline_ + 1) % 262;
    }
}

void Ppu::bindCpu(Cpu& cpu) {
    cpu_ = &cpu;
}

void Ppu::setCtrl(std::uint8_t ctrl) {
    addr_inc_ = (ctrl & 0x04) ? 32 : 1;
    vblank_nmi_ = ctrl & 0x80;
}

void Ppu::setAddr(std::uint8_t addr) {
    addr_ = addr_ << 8 | addr;
    addr_ &= 0x3FFF;
}

void Ppu::setData(std::uint8_t data) {
    ppu_bus_.write(addr_, data);
    addr_ += addr_inc_;
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = vblank_ << 7;
    vblank_ = false;
    return status;
}

std::uint8_t Ppu::getData() {
    std::uint8_t result =  ppu_bus_.read(addr_);
    addr_ += addr_inc_;
    return result;
}