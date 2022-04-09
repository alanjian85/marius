#ifndef NES_CARTRIDGE_H_
#define NES_CARTRIDGE_H_

#include <cstdlib>
#include <iostream>
#include <vector>

namespace nes {
    class Cartridge {
        friend std::istream& operator>>(std::istream& lhs, Cartridge& rhs);
    private:
        std::vector<std::uint8_t> prg_rom_;
        std::vector<std::uint8_t> chr_rom_;
    };

    std::istream& operator>>(std::istream& lhs, Cartridge& rhs);
}

#endif // NES_CARTRIDGE_H_