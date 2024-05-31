#ifndef NES_IO_CARTRIDGE_H_
#define NES_IO_CARTRIDGE_H_

#include <cstdint>
#include <iostream>
#include <vector>

namespace nes {
enum class Mirroring { kHorizontal, kVertical };

class Cartridge final {
    friend std::istream& operator>>(std::istream& lhs, Cartridge& rhs);

   public:
    [[nodiscard]] std::uint8_t getPrgRomBanks() const;

    [[nodiscard]] const std::vector<std::uint8_t>& getPrgRom() const;

    [[nodiscard]] std::uint8_t getChrRomBanks() const;

    [[nodiscard]] const std::vector<std::uint8_t>& getChrRom() const;

    [[nodiscard]] Mirroring getMirroring() const;

    [[nodiscard]] std::uint8_t getMapperNum() const;

   private:
    std::uint8_t prg_rom_banks_;
    std::vector<std::uint8_t> prg_rom_;
    std::uint8_t chr_rom_banks_;
    std::vector<std::uint8_t> chr_rom_;
    Mirroring mirroring_;
    std::uint8_t mapper_num_;
};

std::istream& operator>>(std::istream& lhs, Cartridge& rhs);
}  // namespace nes

#endif  // NES_IO_CARTRIDGE_H_