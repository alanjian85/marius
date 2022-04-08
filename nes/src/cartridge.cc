#include "cartridge.h"
using namespace nes;

#include <array>
#include <stdexcept>
#include <string_view>

std::istream& nes::operator>>(std::istream& lhs, Cartridge& rhs) {
    std::array<char, 16> header;
    if (!lhs.read(header.data(), 16)) {
        throw std::runtime_error("Unable to read cartridge header");
    }

    if (std::string_view(header.begin(), 4) != "NES\x1A") {
        throw std::runtime_error("iNES header constant is not NES\\x1A");
    }

    rhs.prg_rom_.resize(0x4000 * header[4]);
    if (!lhs.read(reinterpret_cast<char*>(rhs.prg_rom_.data()), 0x4000 * header[4])) {
        throw std::runtime_error("Reading PRG ROM image from cartridge failed");
    }

    rhs.chr_rom_.resize(0x2000 * header[5]);
    if (!lhs.read(reinterpret_cast<char*>(rhs.chr_rom_.data()), 0x2000 * header[5])) {
        throw std::runtime_error("Reading CHR ROM image from cartridge failed");
    }

    return lhs;
}