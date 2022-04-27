#include "mapper_nrom.h"
using namespace nes;

#include <spdlog/spdlog.h>

std::string MapperNrom::getName() const {
    return "NROM";
}

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
    spdlog::error("Attempted to write PRG ROM");
}

std::uint8_t MapperNrom::readChr(std::uint16_t addr) {
    if (addr < 0x2000) {
        return cartridge_.getChrRom()[addr];
    }
    return 0x00;
}

void MapperNrom::writeChr(std::uint16_t addr, std::uint8_t val) {
    spdlog::warn("Attempted to write CHR ROM");
}

Mirroring MapperNrom::getMirroring() {
    return cartridge_.getMirroring();
}