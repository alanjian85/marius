#ifndef NES_COMPONENTS_PPU_H_
#define NES_COMPONENTS_PPU_H_

#include <vector>

#include "cpu.h"
#include "io/framebuffer.h"
#include "ppu_bus.h"

namespace nes {
    class Ppu final {
    public:
        static constexpr int kWidth = 256, kHeight = 240;
        static constexpr float kAspect = static_cast<float>(kWidth) / kHeight;        

        Ppu(Framebuffer& framebuffer, PpuBus& bus, Cpu& cpu);

        void reset();

        void cycle();

        void bindCpu(Cpu& cpu);

        void setCtrl(std::uint8_t ctrl);

        void setMask(std::uint8_t mask);

        void setOamAddr(std::uint8_t addr);

        void setOamData(std::uint8_t data);

        void setScroll(std::uint8_t scroll);

        void setAddr(std::uint8_t addr);

        void setData(std::uint8_t data);

        [[nodiscard]] std::uint8_t getStatus();

        [[nodiscard]] std::uint8_t getData();

        [[nodiscard]] std::uint8_t getOamData();
    private:
        Framebuffer& framebuffer_;
        PpuBus& bus_;
        Cpu& cpu_;

        int cycle_;
        int scanline_;
        std::vector<std::uint8_t> scanline_sprites_;
        
        bool show_background_;
        bool show_sprites_;

        std::uint16_t background_pattern_;
        std::uint16_t sprite_pattern_;

        std::uint8_t fine_x_scroll_;

        bool write_toggle_;
        std::uint16_t curr_addr_;
        std::uint16_t temp_addr_;
        std::uint8_t addr_inc_;
        std::uint8_t read_buffer_;

        std::uint8_t oam_addr_;

        bool sprite_zero_;
        bool sprite_overflow_;
        bool vblank_;
        bool vblank_nmi_;
    };
}

#endif // NES_COMPONENTS_PPU_H_