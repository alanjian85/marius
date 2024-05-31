#include "cartridge.h"
using namespace nes;

#include <spdlog/spdlog.h>

#include <array>
#include <string_view>

std::uint8_t Cartridge::getPrgRomBanks() const { return prg_rom_banks_; }

const std::vector<std::uint8_t>& Cartridge::getPrgRom() const {
    return prg_rom_;
}

std::uint8_t Cartridge::getChrRomBanks() const { return chr_rom_banks_; }

const std::vector<std::uint8_t>& Cartridge::getChrRom() const {
    return chr_rom_;
}

Mirroring Cartridge::getMirroring() const { return mirroring_; }

std::uint8_t Cartridge::getMapperNum() const { return mapper_num_; }

std::istream& nes::operator>>(std::istream& lhs, Cartridge& rhs) {
    std::array<char, 16> header;
    if (!lhs.read(header.data(), 16)) {
        spdlog::critical("Failed to read cartridge header");
    }

    if (std::string_view(&header[0], 4) != "NES\x1A") {
        spdlog::warn("iNES header constant is not NES\\x1A");
    }

    rhs.prg_rom_banks_ = header[4];
    rhs.prg_rom_.resize(0x4000 * rhs.prg_rom_banks_);
    spdlog::info("PRG ROM banks: {}", rhs.prg_rom_banks_);
    if (!lhs.read(reinterpret_cast<char*>(rhs.prg_rom_.data()),
                  0x4000 * rhs.prg_rom_banks_)) {
        spdlog::critical("Failed to read PRG ROM image from cartridge");
    }

    rhs.chr_rom_banks_ = header[5];
    rhs.chr_rom_.resize(0x2000 * rhs.chr_rom_banks_);
    spdlog::info("CHR ROM banks: {}", rhs.chr_rom_banks_);
    if (!lhs.read(reinterpret_cast<char*>(rhs.chr_rom_.data()),
                  0x2000 * rhs.chr_rom_banks_)) {
        spdlog::critical("Failed to read CHR ROM image from cartridge");
    }

    if (!(header[6] & 0x01)) {
        rhs.mirroring_ = Mirroring::kHorizontal;
        spdlog::info("Cartridge mirroring: Horizontal");
    } else {
        rhs.mirroring_ = Mirroring::kVertical;
        spdlog::info("Cartridge mirroring: Vertical");
    }

    rhs.mapper_num_ = (header[6] & 0xF0) >> 4 | header[7] & 0xF0;
    spdlog::info("Mapper number: {:03}", rhs.mapper_num_);

    return lhs;
}