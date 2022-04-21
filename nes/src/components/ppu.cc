#include "ppu.h"
using namespace nes;

#include <cassert>

#include "cpu.h"
#include "palette.h"

Ppu::Ppu(Framebuffer& framebuffer, PpuBus& bus, Cpu& cpu)
    : framebuffer_(framebuffer),
      bus_(bus),
      cpu_(cpu)
{
    cycle_ = 0;
    scanline_ = 261;

    show_background_ = false;
    show_sprites_ = false;

    nametable_ = 0x2000;
    background_pattern_ = 0x0000;
    sprite_pattern_ = 0x0000;

    addr_ = 0x00;
    addr_inc_ = 1;
    read_buffer_ = 0x00;

    sprite_zero_ = false;
    sprite_overflow_ = false;
    vblank_ = true;
    vblank_nmi_ = false;
}

void Ppu::reset() {
    cycle_ = 0;
    scanline_ = 261;

    show_background_ = false;
    show_sprites_ = false;
    
    nametable_ = 0x2000;
    background_pattern_ = 0x0000;
    sprite_pattern_ = 0x0000;

    addr_inc_ = 1;

    vblank_nmi_ = false;
}

void Ppu::cycle() {
    if (scanline_ == 261 && cycle_ == 0) {
        sprite_zero_ = false;
        sprite_overflow_ = false;
        vblank_ = false;
        framebuffer_.lock();
    }

    if (scanline_ < 240) {
        for (int x = 0; x < 256; ++x) {
            if (cycle_ == 0) {
                std::uint8_t background_index;

                if (show_background_) {
                    int tile_x = x / 8, tile_y = scanline_ / 8;
                    int block_x = x / 32, block_y = scanline_ / 32;

                    std::uint8_t tile = bus_.read(nametable_ + tile_y * 32 + tile_x);
                    std::uint8_t attribute = bus_.read(nametable_ + 0x03C0 + block_y * 8 + block_x);
                    std::uint8_t palette;
                    if (tile_x % 4 < 2 && tile_y % 4 < 2) {
                        palette = attribute >> 0 & 0x03;
                    } else if (tile_x % 4 >= 2 && tile_y % 4 < 2) {
                        palette = attribute >> 2 & 0x03;
                    } else if (tile_x % 4 < 2 && tile_y % 4 >= 2) {
                        palette = attribute >> 4 & 0x03;
                    } else {
                        palette = attribute >> 6 & 0x03;
                    }

                    bool bit0 = bus_.read(background_pattern_ + tile * 16 + scanline_ % 8) & 0x80 >> x % 8;
                    bool bit1 = bus_.read(background_pattern_ + tile * 16 + 8 + scanline_ % 8) & 0x80 >> x % 8;
                    background_index = bit0 | bit1 << 1;

                    if (background_index != 0x00) {
                        framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F00 + palette * 4 + background_index)]);
                    } else {
                        framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F00)]);
                    }
                }

                if (show_sprites_) {
                    for (auto sprite : scanline_sprites_) {
                        std::uint8_t sprite_y = bus_.readOam(sprite * 4 + 0);
                        std::uint8_t tile = bus_.readOam(sprite * 4 + 1);
                        std::uint8_t attribute = bus_.readOam(sprite * 4 + 2);
                        std::uint8_t sprite_x = bus_.readOam(sprite * 4 + 3);
                    
                        if (x - sprite_x >= 0 && x - sprite_x < 8) {
                            std::uint8_t x_offset = (x - sprite_x) % 8;
                            std::uint8_t y_offset = (scanline_ - 1 - sprite_y) % 8;

                            if (attribute & 0x40) {
                                x_offset = 7 - x_offset;
                            }

                            if (attribute & 0x80) {
                                y_offset = 7 - y_offset;
                            }

                            std::uint8_t palette = attribute & 0x03;

                            bool bit0 = bus_.read(sprite_pattern_ + tile * 16 + y_offset) & 0x80 >> x_offset % 8;
                            bool bit1 = bus_.read(sprite_pattern_ + tile * 16 + 8 + y_offset) & 0x80 >> x_offset % 8;
                            std::uint8_t sprite_index = bit0 | bit1 << 1;

                            if (sprite_index != 0x00) {
                                framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F10 + palette * 4 + sprite_index)]);
                                
                                if (sprite == 0 && show_background_ && background_index != 0) {
                                    sprite_zero_ = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (cycle_ == 340) {
            scanline_sprites_.clear();

            for (std::uint8_t i = 0; i < 64; ++i) {
                std::uint8_t sprite_y = bus_.readOam(i * 4);
                if (scanline_ - sprite_y >= 0 && scanline_ - sprite_y < 8) {
                    scanline_sprites_.push_back(i);
                }
            }

            if (scanline_sprites_.size() > 8) {
                sprite_overflow_ = true;
            }
        }
    }

    if (scanline_ == 241 && cycle_ == 0) {
        vblank_ = true;
        if (vblank_nmi_) {
            cpu_.nmi();
        }
        framebuffer_.unlock();
    }

    ++cycle_;
    if (cycle_ == 341) {
        cycle_ = 0;
        scanline_ = (scanline_ + 1) % 262;
    }
}

void Ppu::setCtrl(std::uint8_t ctrl) {
    switch (ctrl & 0x03) {
        case 0x00:
            nametable_ = 0x2000;
            break;
        case 0x01:
            nametable_ = 0x2400;
            break;
        case 0x02:
            nametable_ = 0x2800;
            break;
        case 0x03:
            nametable_ = 0x2C00;
            break;
    }
    background_pattern_ = (ctrl & 0x10) ? 0x1000 : 0x0000;
    sprite_pattern_ = (ctrl & 0x08) ? 0x1000 : 0x0000;
    addr_inc_ = (ctrl & 0x04) ? 32 : 1;
    vblank_nmi_ = ctrl & 0x80;
}

void Ppu::setMask(std::uint8_t mask) {
    show_background_ = mask & 0x08;
    show_sprites_ = mask & 0x10;
}

void Ppu::setOamAddr(std::uint8_t addr) {
    oam_addr_ = addr;
}

void Ppu::setOamData(std::uint8_t data) {
    bus_.writeOam(oam_addr_++, data);
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
    std::uint8_t status = 
        vblank_ << 7 | 
        sprite_zero_ << 6 | 
        sprite_overflow_ << 5;
    vblank_ = false;
    return status;
}

std::uint8_t Ppu::getData() {
    std::uint8_t result = bus_.read(addr_);
    addr_ += addr_inc_;
    if (addr_ < 0x3F00) {
        std::swap(result, read_buffer_);
    }
    return result;
}

std::uint8_t Ppu::getOamData() {
    return bus_.readOam(oam_addr_++);
}