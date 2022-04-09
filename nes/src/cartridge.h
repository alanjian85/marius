#ifndef NES_CARTRIDGE_H_
#define NES_CARTRIDGE_H_

#include <cstdint>
#include <iostream>
#include <vector>

namespace nes {
    class Cartridge {
        friend std::istream& operator>>(std::istream& lhs, Cartridge& rhs);
    public:
        [[nodiscard]] std::uint8_t getMapperNum() const;

        [[nodiscard]] const std::vector<std::uint8_t>& getPrgRom() const;

        [[nodiscard]] const std::vector<std::uint8_t>& getChrRom() const;
    private:
        std::uint8_t mapper_num_;
        std::vector<std::uint8_t> prg_rom_;
        std::vector<std::uint8_t> chr_rom_;
    };

    std::istream& operator>>(std::istream& lhs, Cartridge& rhs);
}

#endif // NES_CARTRIDGE_H_