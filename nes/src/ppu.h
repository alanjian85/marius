#ifndef NES_PPU_H_
#define NES_PPU_H_

#include "framebuffer.h"

namespace nes {
    class Ppu final {
    public:
        static constexpr int kWidth = 256, kHeight = 240;
        static constexpr float kAspect = static_cast<float>(kWidth) / kHeight;        

        Ppu(Framebuffer& framebuffer);

        [[nodiscard]] std::uint8_t getStatus();

        void update();
    private:
        Framebuffer& framebuffer_;

        bool vblank_;
    };
}

#endif // NES_PPU_H_