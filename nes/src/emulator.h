#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>

#include "graphics/framebuffer.h"
#include "graphics/renderer.h"
#include "graphics/window.h"
#include "io/cartridge.h"
#include "io/controller.h"
#include "mappers/mapper.h"
#include "settings.h"

namespace nes {
    class Emulator final {
    public:
        Emulator(const std::filesystem::path& path, Settings settings);

        void run();
    private:
        using Clock = std::chrono::high_resolution_clock;

        std::chrono::nanoseconds cycle_interval_;

        Settings settings_;
        Cartridge cartridge_;
        Controller controller1_;
        Controller controller2_;
        std::unique_ptr<Mapper> mapper_;

        Window window_;
        Renderer renderer_;
        Framebuffer framebuffer_;
    };
}

#endif // NES_EMULATOR_H_