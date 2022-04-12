#include "ppu_bus.h"
using namespace nes;

PpuBus::PpuBus(Mapper& mapper)
    : mapper_(mapper)
{

}

std::uint8_t PpuBus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return mapper_.readChr(addr);
    } else if (addr < 0x3F00) {
        std::uint16_t nametable = addr & 0xFC00;
        switch (mapper_.getMirroring()) {
            case Mirroring::kHorizontal:
                if (nametable == 0x2000 || nametable == 0x2400) {
                    return ram_[addr & 0x3FF];
                } else {
                    return ram_[addr & 0x3FF + 0x400];
                }
            case Mirroring::kVertical:
                if (nametable == 0x2000 || nametable == 0x2800) {
                    return ram_[addr & 0x3FF];
                } else {
                    return ram_[addr & 0x3FF + 0x400];
                }
        }
    } else {
        return ram_[addr & 0x001F];
    }
    return 0;
}

void PpuBus::write(std::uint16_t addr, std::uint8_t val) {
    if (addr < 0x2000) {
        mapper_.writeChr(addr, val);
    } else if (addr < 0x3F00) {
        std::uint16_t nametable = addr & 0xFC00;
        switch (mapper_.getMirroring()) {
            case Mirroring::kHorizontal:
                if (nametable == 0x2000 || nametable == 0x2400) {
                    ram_[addr & 0x3FF] = val;
                } else {
                    ram_[addr & 0x3FF + 0x400] = val;
                }
            case Mirroring::kVertical:
                if (nametable == 0x2000 || nametable == 0x2800) {
                    ram_[addr & 0x3FF] = val;
                } else {
                    ram_[addr & 0x3FF + 0x400] = val;
                }
        }
    } else {
        palette_[addr & 0x001F] = val;
        std::uint8_t nibble = addr & 0xF;
        if (nibble == 0x0 || nibble == 0x4 || nibble == 0x8 || nibble == 0xC) {
            if (addr & 0x0010) {
                palette_[nibble] = val;
            } else {
                palette_[nibble | 0x0010] = val;
            }
        }
    }
}

std::uint8_t PpuBus::readOam(std::uint8_t addr) const {
    return oam_[addr];
}

void PpuBus::writeOam(std::uint8_t addr, std::uint8_t val) {
    oam_[addr] = val;
}