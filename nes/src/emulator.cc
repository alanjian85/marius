#include "emulator.h"
using namespace nes;

Emulator::Emulator()
    : cpu_(cpu_bus_)
{
    cycle_interval_ = std::chrono::nanoseconds(559);
}

void Emulator::run(std::istream& rom) {
    rom >> cartridge_;
    cpu_.reset();

    prev_time_ = Clock::now();
    elapsed_time_ = prev_time_ - prev_time_;
    for (;;) {
        elapsed_time_ += Clock::now() - prev_time_;
        while (elapsed_time_ > cycle_interval_) {
            cpu_.cycle();
            elapsed_time_ -= cycle_interval_;
        }
        prev_time_ = Clock::now();

        std::cout << '\r';
        for (std::uint16_t i = 0; i < 12; ++i) {
            std::cout << static_cast<char>(cpu_bus_.read(0x100 + i));
        }
    }
}