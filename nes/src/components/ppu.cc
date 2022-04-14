#include "ppu.h"
using namespace nes;

#include <cassert>

#include "cpu.h"
#include "palette.h"

Ppu::Ppu(Framebuffer& framebuffer, PpuBus& bus)
    : framebuffer_(framebuffer),
      bus_(bus)
{
    cycle_ = 0;
    scanline_ = 261;

    show_background_ = false;

    addr_ = 0x00;
    addr_inc_ = 1;

    vblank_ = true;
    vblank_nmi_ = false;
}

void Ppu::reset() {
    cycle_ = 0;
    scanline_ = 261;

    show_background_ = false;

    addr_inc_ = 1;

    vblank_ = true;
    vblank_nmi_ = false;
}

void Ppu::cycle() {
    if (show_background_ && cycle_ == 0 && scanline_ < 240) {
        for (int x = 0; x < 256; ++x) {
            int tile_x = x / 8, tile_y = scanline_ / 8;
            int block_x = x / 32, block_y = scanline_ / 32;

            std::uint8_t tile = bus_.read(0x2000 + tile_y * 32 + tile_x);
            std::uint8_t block = bus_.read(0x23C0 + block_y * 8 + block_x);
            std::uint8_t palette = 0x00;
            if (tile_x % 4 < 2 && tile_y % 4 < 2) {
                palette = block >> 0 & 0x03;
            } else if (tile_x % 4 >= 2 && tile_y % 4 < 2) {
                palette = block >> 2 & 0x03;
            } else if (tile_x % 4 < 2 && tile_y % 4 >= 2) {
                palette = block >> 4 & 0x03;
            } else {
                palette = block >> 6 & 0x03;
            }

            bool bit0 = bus_.read(0x1000 + tile * 16 + scanline_ % 8) & 0x80 >> x % 8;
            bool bit1 = bus_.read(0x1000 + tile * 16 + 8 + scanline_ % 8) & 0x80 >> x % 8;
            std::uint8_t index = bit0 | bit1 << 1;

            if (index != 0x00) {
                framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F00 + palette * 4 + index)]);
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

void Ppu::setMask(std::uint8_t mask) {
    show_background_ = mask & 0x08;
}

void Ppu::setAddr(std::uint8_t addr) {
    addr_ = addr_ << 8 | addr;
    addr_ &= 0x3FFF;
}

void Ppu::setData(std::uint8_t data) {
    bus_.write(addr_, data);
    addr_ += addr_inc_;
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = vblank_ << 7;
    vblank_ = false;
    return status;
}

std::uint8_t Ppu::getData() {
    std::uint8_t result =  bus_.read(addr_);
    addr_ += addr_inc_;
    return result;
}