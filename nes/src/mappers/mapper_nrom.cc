#include "mapper_nrom.h"
using namespace nes;

std::uint8_t MapperNrom::readPrg(std::uint16_t addr) {
    if (addr >= 0x8000) {
        if (cartridge_.getPrgRomBanks() == 1) {
            return cartridge_.getPrgRom()[addr & 0x3FFF];
        } else {
            return cartridge_.getPrgRom()[addr - 0x8000];
        }
    }
    return 0x00;
}

void MapperNrom::writePrg(std::uint16_t addr, std::uint8_t val) {
    throw std::logic_error("Error: Attempted to write to PRG ROM in NROM");
}

std::uint8_t MapperNrom::readChr(std::uint16_t addr) {
    if (addr < 0x2000) {
        return cartridge_.getChrRom()[addr];
    }
    return 0x00;
}

void MapperNrom::writeChr(std::uint16_t addr, std::uint8_t val) {
    throw std::logic_error("Error: Attempted to write to CHR ROM in NROM");
}

Mirroring MapperNrom::getMirroring() {
    return cartridge_.getMirroring();
}