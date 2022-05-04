#ifndef NES_IO_CONTROLLER_H_
#define NES_IO_CONTROLLER_H_

#include <cstdint>

#include "controller_keymap.h"

namespace nes {
    class Controller final {
    public:
        Controller(const ControllerKeymap& keymap);

        void write(std::uint8_t val);

        [[nodiscard]] std::uint8_t read();
    private:
        void update();

        const ControllerKeymap& keymap_;

        bool strobe_;
        std::uint8_t state_;
    };
}

#endif // NES_IO_CONTROLLER_H_