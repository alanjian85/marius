#ifndef NES_PPU_LATCH_H_
#define NES_PPU_LATCH_H_

#include <cstdint>

#include "ppu.h"

namespace nes {
    class PpuLatch final {
    public:
        PpuLatch(Ppu& ppu);

        void write(std::uint16_t addr, std::uint8_t val);

        [[nodiscard]] std::uint8_t read(std::uint16_t addr);
    private:
        Ppu& ppu_;
        std::uint8_t val_;
    };
}

#endif // NES_PPU_LATCH_H_