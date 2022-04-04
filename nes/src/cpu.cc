#include "cpu.h"
using namespace nes;

CPU::CPU(Bus& bus) 
    : bus_(bus)
{

}

void CPU::step() {
    auto opcode = fetch();
    if (execImplied(opcode))
        return;
    if (execGroup0(opcode))
        return;
    if (execGroup1(opcode))
        return;
    if (execGroup2(opcode))
        return;
}

bool CPU::execBranch(std::uint8_t opcode) {
    if (opcode & 0x10) {
        auto condition = opcode & 0x20;
        switch ((opcode & 0xC0) >> 6) {
            case 0b00: // negative
                break;
            case 0b01: // overflow
                break;
            case 0b10: // carry
                break;
            case 0b11: // zero
                break;
        }
    }
    return false;
}

bool CPU::execImplied(std::uint8_t opcode) {
    switch (opcode) {
        case 0x00: // BRK
            break;
        case 0x20: // JSR
            break;
        case 0x40: // RTI
            break;
        case 0x60: // RTS
            break;
        case 0x08: // PHP
            break;
        case 0x18: // CLC
            break;
        case 0x28: // PLP
            break;
        case 0x38: // SEC
            break;
        case 0x48: // PHA
            break;
        case 0x58: // CLI
            break;
        case 0x68: // PLA
            break;
        case 0x78: // SEI
            break;
        case 0x88: // DEY
            break;
        case 0x98: // TYA
            break;
        case 0xA8: // TAY
            break;
        case 0xB8: // CLV
            break;
        case 0xC8: // INY
            break;
        case 0xD8: // CLD
            break;
        case 0xE8: // INX
            break;
        case 0xF8: // SED
            break;
        case 0x8A: // TXA
            break;
        case 0x9A: // TXS
            break;
        case 0xAA: // TAX
            break;
        case 0xBA: // TSX
            break;
        case 0xCA: // DEX
            break;
        case 0xEA: // NOP
            break;
        default:
            return false;
    }
    return true;
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