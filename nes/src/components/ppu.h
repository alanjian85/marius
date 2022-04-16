#ifndef NES_COMPONENTS_PPU_H_
#define NES_COMPONENTS_PPU_H_

#include "io/framebuffer.h"
#include "ppu_bus.h"

namespace nes {
    class Cpu;

    class Ppu final {
    public:
        static constexpr int kWidth = 256, kHeight = 240;
        static constexpr float kAspect = static_cast<float>(kWidth) / kHeight;        

        Ppu(Framebuffer& framebuffer, PpuBus& bus);

        void reset();

        void cycle();

        void bindCpu(Cpu& cpu);

        void setCtrl(std::uint8_t ctrl);

        void setMask(std::uint8_t mask);

        void setAddr(std::uint8_t addr);

        void setData(std::uint8_t data);

        void setOamAddr(std::uint8_t addr);

        void setOamData(std::uint8_t data);

        [[nodiscard]] std::uint8_t getStatus();

        [[nodiscard]] std::uint8_t getData();

        [[nodiscard]] std::uint8_t getOamData();
    private:
        Framebuffer& framebuffer_;
        PpuBus& bus_;
        Cpu* cpu_;

        int cycle_;
        int scanline_;

        bool show_background_;

        std::uint16_t addr_;
        std::uint8_t addr_inc_;

        std::uint8_t oam_addr_;

        bool vblank_;
        bool vblank_nmi_;
    };
}

#endif // NES_COMPONENTS_PPU_H_