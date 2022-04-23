#ifndef NES_IO_CONTROLLER_H_
#define NES_IO_CONTROLLER_H_

#include <cstdint>

#include "keymap.h"

namespace nes {
    class Controller final {
    public:
        Controller(Keymap& keymap);

        void write(std::uint8_t val);

        [[nodiscard]] std::uint8_t read();
    private:
        void update();

        Keymap& keymap_;

        bool strobe_;
        std::uint8_t state_;
    };
}

#endif // NES_IO_CONTROLLER_H_