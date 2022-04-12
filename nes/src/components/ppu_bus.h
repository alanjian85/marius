#ifndef PPU_BUS_H_
#define PPU_BUS_H_

#include <array>
#include <cstdint>

#include "mappers/mapper.h"

namespace nes {
    class PpuBus {
    public:
        explicit PpuBus(Mapper& mapper);

        [[nodiscard]] std::uint8_t read(std::uint16_t addr) const;

        void write(std::uint16_t addr, std::uint8_t val);
    
        [[nodiscard]] std::uint8_t readOam(std::uint8_t addr) const;

        void writeOam(std::uint8_t addr, std::uint8_t val);
    private:
        Mapper& mapper_;
        std::array<std::uint8_t, 0x800> ram_;
        std::array<std::uint32_t, 0xFF> oam_;
    };
}

#endif // PPU_BUS_H_