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

    background_pattern_ = 0x0000;
    sprite_pattern_ = 0x0000;

    fine_x_scroll_ = 0x00;

    write_toggle_ = false;
    curr_addr_ = 0x00;
    temp_addr_ = 0x00;
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
    
    background_pattern_ = 0x0000;
    sprite_pattern_ = 0x0000;

    fine_x_scroll_ = 0x00;

    write_toggle_ = false;
    temp_addr_ = 0x00;
    addr_inc_ = 1;

    vblank_nmi_ = false;
}

void Ppu::cycle() {
    if (scanline_ == 261) {
        if (cycle_ == 0) {
            sprite_zero_ = false;
            sprite_overflow_ = false;
            vblank_ = false;
            framebuffer_.lock();
            scanline_sprites_.clear();
        }

        if (cycle_ == 257 && (show_background_ || show_sprites_)) {
            curr_addr_ &= ~0x041F;
            curr_addr_ |= temp_addr_ & 0x041F;
        }

        if (cycle_ >= 284  && cycle_ <= 304 && (show_background_ && show_sprites_)) {
            curr_addr_ &= ~0x7BE0;
            curr_addr_ |= temp_addr_ & 0x7BE0;
        }
    }

    if (scanline_ < 240) {
        if (cycle_ >= 1 && cycle_ <= 256) {
            int x = cycle_ - 1;

            std::uint8_t background_index;

            if (show_background_) {
                int fine_x = (fine_x_scroll_ + x) & 0x07;

                std::uint8_t tile = bus_.read(0x2000 | (curr_addr_ & 0x0FFF));
                std::uint8_t attribute = bus_.read(0x23c0 | 
                    (curr_addr_ & 0x0C00) |
                    ((curr_addr_ & 0x0380) >> 4) |
                    ((curr_addr_ & 0x001C) >> 2)
                );

                std::uint8_t bit0 = (bus_.read(background_pattern_ | tile * 16 + ((curr_addr_ & 0x7000) >> 12)) & 0x80 >> fine_x) >> (7 - fine_x);
                std::uint8_t bit1 = (bus_.read(background_pattern_ | tile * 16 + 8 + ((curr_addr_ & 0x7000) >> 12)) & 0x80 >> fine_x) >> (7 - fine_x);
                background_index = bit0 | bit1 << 1;

                int shift = ((curr_addr_ & 0x0040) >> 4) | (curr_addr_ & 0x0002);
                std::uint8_t palette = attribute >> shift & 0x03;
                if (background_index != 0x00) {
                    framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F00 + palette * 4 + background_index)]);
                } else {
                    framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F00)]);
                }

                if (fine_x == 7) {
                    if ((curr_addr_ & 0x001F) == 31) {
                        curr_addr_ &= ~0x001F;
                        curr_addr_ ^= 0x0400;
                    } else {
                        ++curr_addr_;
                    }
                }
            }

            if (show_sprites_) {
                for (auto sprite : scanline_sprites_) {
                    std::uint8_t sprite_y = bus_.readOam(sprite * 4 + 0);
                    std::uint8_t tile = bus_.readOam(sprite * 4 + 1);
                    std::uint8_t attribute = bus_.readOam(sprite * 4 + 2);
                    std::uint8_t sprite_x = bus_.readOam(sprite * 4 + 3);
                
                    if (x - sprite_x >= 0 && x - sprite_x < 8) {
                        std::uint8_t x_offset = (x - sprite_x) & 0x07;
                        std::uint8_t y_offset = (scanline_ - 1 - sprite_y) & 0x07;

                        if (attribute & 0x40) {
                            x_offset = 7 - x_offset;
                        }

                        if (attribute & 0x80) {
                            y_offset = 7 - y_offset;
                        }

                        std::uint8_t palette = attribute & 0x03;

                        int shift = x_offset % 8;
                        std::uint8_t bit0 = (bus_.read(sprite_pattern_ + tile * 16 + y_offset) & 0x80 >> shift) >> (7 - shift);
                        std::uint8_t bit1 = (bus_.read(sprite_pattern_ + tile * 16 + 8 + y_offset) & 0x80 >> shift) >> (7 - shift);
                        std::uint8_t sprite_index = bit0 | bit1 << 1;

                        if (sprite_index != 0x00) {
                            if (sprite == 0 && show_background_ && background_index != 0x00) {
                                sprite_zero_ = true;
                            }
                            
                            if (!(attribute & 0x20) || !show_background_ || background_index == 0x00) {
                                framebuffer_.setPixel(x, scanline_, kPalette[bus_.read(0x3F10 + palette * 4 + sprite_index)]);
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (cycle_ == 256 && (show_background_ || show_sprites_)) {
            if ((curr_addr_ & 0x7000) != 0x7000) {
                curr_addr_ += 0x1000;
            } else {
                curr_addr_ &= ~0x7000;
                int y = (curr_addr_ & 0x03E0) >> 5;
                if (y == 29) {
                    y = 0;
                    curr_addr_ ^= 0x0800;
                } else if (y == 31) {
                    y = 0;
                } else {
                    ++y;
                }
                curr_addr_ = (curr_addr_ & ~0x03E0) | (y << 5);
            }
        }

        if (cycle_ == 257 && (show_background_ || show_sprites_)) {
            curr_addr_ &= ~0x041F;
            curr_addr_ |= temp_addr_ & 0x041F;
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
    temp_addr_ &= ~0x0C00;
    temp_addr_ |= (ctrl & 0x03) << 10;
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

void Ppu::setScroll(std::uint8_t scroll) {
    if (!write_toggle_) {
        temp_addr_ &= ~0x001F;
        temp_addr_ |= (scroll >> 3) & 0x1F;
        fine_x_scroll_ = scroll & 0x07;
        write_toggle_ = true;
    } else {
        temp_addr_ &= ~0x73E0;
        temp_addr_ |= (scroll & 0x07) << 12;
        temp_addr_ |= (scroll & 0xF8) << 2;
        write_toggle_ = false;
    }
}

void Ppu::setAddr(std::uint8_t addr) {
    if (!write_toggle_) {
        temp_addr_ &= ~0xFF00;
        temp_addr_ |= (addr & 0x3F) << 8;
        write_toggle_ = true;
    } else {
        temp_addr_ &= ~0x00FF;
        temp_addr_ |= addr;
        curr_addr_ = temp_addr_;
        write_toggle_ = false;
    }
}

void Ppu::setData(std::uint8_t data) {
    bus_.write(curr_addr_, data);
    curr_addr_ += addr_inc_;
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = 
        vblank_ << 7 | 
        sprite_zero_ << 6 | 
        sprite_overflow_ << 5;
    vblank_ = false;
    write_toggle_ = false;
    return status;
}

std::uint8_t Ppu::getData() {
    std::uint8_t result = bus_.read(curr_addr_);
    curr_addr_ += addr_inc_;
    if (curr_addr_ < 0x3F00) {
        std::swap(result, read_buffer_);
    }
    return result;
}

std::uint8_t Ppu::getOamData() {
    return bus_.readOam(oam_addr_++);
}