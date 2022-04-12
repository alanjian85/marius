#ifndef NES_PPU_H_
#define NES_PPU_H_

#include "framebuffer.h"
#include "ppu_bus.h"

namespace nes {
    class Ppu final {
    public:
        static constexpr int kWidth = 256, kHeight = 240;
        static constexpr float kAspect = static_cast<float>(kWidth) / kHeight;        

        Ppu(Framebuffer& framebuffer, PpuBus& ppu_bus);

        void reset();

        void cycle();

        [[nodiscard]] std::uint8_t getStatus();
    private:
        Framebuffer& framebuffer_;
        PpuBus& ppu_bus_;

        int cycle_;
        int scanline_;
        bool vblank_;
    };
}

#endif // NES_PPU_H_