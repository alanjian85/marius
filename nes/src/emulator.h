#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <filesystem>
#include <fstream>

#include "settings.h"

namespace nes {
    class Emulator final {
    public:
        Emulator(const std::filesystem::path& path, Settings settings);

        Emulator(const Emulator&) = delete;

        Emulator& operator=(const Emulator&) = delete;

        ~Emulator();

        void run();
    private:
        using Clock = std::chrono::high_resolution_clock;

        int width_, height_;
        std::chrono::nanoseconds cycle_interval_;

        std::ifstream rom_;

        SDL_Window* window_;
        SDL_Renderer* renderer_;

        Keymap keymap1_;
        Keymap keymap2_;
    };
}

#endif // NES_EMULATOR_H_