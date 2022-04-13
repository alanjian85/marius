#ifndef NES_PPU_H_
#define NES_PPU_H_

#include "io/framebuffer.h"
#include "ppu_bus.h"

namespace nes {
    class Cpu;

    class Ppu final {
    public:
        static constexpr int kWidth = 256, kHeight = 240;
        static constexpr float kAspect = static_cast<float>(kWidth) / kHeight;        

        Ppu(Framebuffer& framebuffer, PpuBus& ppu_bus);

        void reset();

        void cycle();

        void bindCpu(Cpu& cpu);

        void setCtrl(std::uint8_t ctrl);

        void setAddr(std::uint8_t addr);

        void setData(std::uint8_t data);

        [[nodiscard]] std::uint8_t getStatus();

        [[nodiscard]] std::uint8_t getData();
    private:
        Framebuffer& framebuffer_;
        PpuBus& ppu_bus_;
        Cpu* cpu_;

        int cycle_;
        int scanline_;

        std::uint8_t addr_inc_;
        bool vblank_nmi_;
        std::uint16_t addr_;

        bool vblank_;
    };
}

#endif // NES_PPU_H_