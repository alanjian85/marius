#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <filesystem>
#include <fstream>

#include "graphics/renderer.h"
#include "graphics/window.h"
#include "io/cartridge.h"
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

        Window window_;
        Renderer renderer_;
    };
}

#endif // NES_EMULATOR_H_