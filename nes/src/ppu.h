#ifndef NES_PPU_H_
#define NES_PPU_H_

#include "framebuffer.h"

namespace nes {
    class Ppu final {
    public:
        static constexpr int kWidth = 256, kHeight = 240;
        static constexpr float kAspect = static_cast<float>(kWidth) / kHeight;        

        Ppu(Framebuffer& framebuffer);

        void reset();

        void dot();

        [[nodiscard]] std::uint8_t getStatus();
    private:
        Framebuffer& framebuffer_;

        int dots_;
        bool vblank_;
    };
}

#endif // NES_PPU_H_