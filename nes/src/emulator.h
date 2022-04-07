#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include "bus.h"
#include "cpu.h"

namespace nes {
    class Emulator {
    public:
        Emulator();

        void run();
    private:
        Bus bus_;
        Cpu cpu_;
    };
}

#endif // NES_EMULATOR_H_