#include "cpu.h"
using namespace nes;

CPU::CPU(Bus& bus) 
    : bus_(bus)
{

}

void CPU::step() {
    auto opcode = fetch();
    if (execGroup0(opcode))
        return;
    if (execGroup1(opcode))
        return;
    if (execGroup2(opcode))
        return;
}

bool CPU::execGroup0(std::uint8_t opcode) {
    if ((opcode & 0x03) == 0x00) {
        switch ((opcode & 0xE0) >> 5) {
            case 0b001: // BIT
                break;
            case 0b010: // JMP
                break;
            case 0b011: // JMP
                break;
            case 0b100: // STY
                break;
            case 0b101: // LDY
                break;
            case 0b110: // CPY
                break;
            case 0b111: // CPX
                break;
        }
        return true;
    }
    return false;
}

bool CPU::execGroup1(std::uint8_t opcode) {
    if ((opcode & 0x03) == 0x01) {
        switch ((opcode & 0xE0) >> 5) {
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
        return true;
    }
    return false;
}

bool CPU::execGroup2(std::uint8_t opcode) {
    if ((opcode & 0x03) == 0x02) {
        switch ((opcode & 0xE0) >> 5) {
            case 0b000: // ASL
                break;
            case 0b001: // ROL
                break;
            case 0b010: // LSR
                break;
            case 0b011: // ROR
                break;
            case 0b100: // STX
                break;
            case 0b101: // LDX
                break;
            case 0b110: // DEC
                break;
            case 0b111: // INC
                break;
        }
        return true;
    }
    return false;
}

std::uint8_t CPU::fetch() {
    return bus_.read(pc_++);
}