#include "cpu.h"
using namespace nes;

CPU::CPU(Bus& bus) 
    : bus_(bus)
{

}

void CPU::execGroup1(std::uint8_t opcode) {
    if (opcode & 0x03 == 0x01) {
        switch (opcode & 0xE0 >> 5) {
            case 0b000: // ORA
                break;
            case 0b001: // AND
                break;
            case 0b010: // EOR
                break;
            case 0b011: // ADC
                break;
            case 0b100: // STA
                break;
            case 0b101: // LDA
                break;
            case 0b110: // CMP
                break;
            case 0b111: // SBC
                break;
        }
    }
}