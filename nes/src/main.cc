#include <iostream>

#include "bus.h"
#include "cpu.h"
using namespace nes;

int main() {
    const std::uint8_t binary[] = {
        0xa9, 0x48, 0x8d, 0x00, 0x01, 0xa9, 0x65, 0x8d, 0x01, 0x01, 0xa9, 0x6c, 0x8d, 0x02, 0x01, 0xa9, 
        0x6c, 0x8d, 0x03, 0x01, 0xa9, 0x6f, 0x8d, 0x04, 0x01, 0xa9, 0x20, 0x8d, 0x05, 0x01, 0xa9, 0x77, 
        0x8d, 0x06, 0x01, 0xa9, 0x6f, 0x8d, 0x07, 0x01, 0xa9, 0x72, 0x8d, 0x08, 0x01, 0xa9, 0x6c, 0x8d,
        0x09, 0x01, 0xa9, 0x64, 0x8d, 0x0a, 0x01, 0xa9, 0x21, 0x8d, 0x0b, 0x01, 
    };

    Bus bus;
    CPU cpu(bus);
    cpu.reset();
    for (std::uint16_t i = 0; i < sizeof(binary); ++i) {
        bus.write(i, binary[i]);
    }
    for (int i = 0; i < 24; ++i) {
        cpu.step();
    }
    for (std::uint16_t i = 0; i < 12; ++i) {
        std::cout << static_cast<char>(bus.read(0x100 + i));
    }
    std::cout << '\n';
    return 0;
}