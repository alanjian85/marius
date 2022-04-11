#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <iostream>

namespace nes {
    class Emulator final {
    public:
        Emulator();

        Emulator(const Emulator&) = delete;

        Emulator& operator=(const Emulator&) = delete;

        ~Emulator();

        void run(std::istream& rom);
    private:
        using Clock = std::chrono::high_resolution_clock;

        std::chrono::nanoseconds cpu_interval_;
        std::chrono::nanoseconds ppu_interval_;
    };
}

#endif // NES_EMULATOR_H_