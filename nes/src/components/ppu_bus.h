#ifndef PPU_COMPONENTS_BUS_H_
#define PPU_COMPONENTS_BUS_H_

#include <array>
#include <cstdint>

#include "mappers/mapper.h"

namespace nes {
class PpuBus final {
   public:
    void setMapper(Mapper& mapper);

    [[nodiscard]] std::uint8_t read(std::uint16_t addr) const;

    void write(std::uint16_t addr, std::uint8_t val);

    [[nodiscard]] std::uint8_t readOam(std::uint8_t addr) const;

    void writeOam(std::uint8_t addr, std::uint8_t val);

   private:
    std::array<std::uint8_t, 0x800> ram_;
    std::array<std::uint8_t, 0x20> palette_;
    std::array<std::uint32_t, 0x100> oam_;

    Mapper* mapper_;
};
}  // namespace nes

#endif  // PPU_COMPONENTS_BUS_H_