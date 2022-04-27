#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <iostream>

#include "io/keymap.h"

namespace nes {
    class Emulator final {
    public:
        Emulator(Keymap keymap1, Keymap keymap2);

        Emulator(const Emulator&) = delete;

        Emulator& operator=(const Emulator&) = delete;

        ~Emulator();

        void run(std::istream& rom);
    private:
        using Clock = std::chrono::high_resolution_clock;

        int width_, height_;
        std::chrono::nanoseconds cycle_interval_;

        SDL_Window* window_;
        SDL_Renderer* renderer_;

        Keymap keymap1_;
        Keymap keymap2_;
    };
}

#endif // NES_EMULATOR_H_