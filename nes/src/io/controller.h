#ifndef NES_IO_CONTROLLER_H_
#define NES_IO_CONTROLLER_H_

#include <cstdint>

namespace nes {
    class Controller {
    public:
        void write(std::uint8_t val);

        [[nodiscard]] std::uint8_t read();
    private:
        void update();

        bool strobe_;
        std::uint8_t state_;
    };
}

#endif // NES_IO_CONTROLLER_H_