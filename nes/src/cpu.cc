#include "cpu.h"
using namespace nes;

CPU::CPU(Bus& bus) 
    : bus_(bus)
{

}

void CPU::step() {
    auto opcode = bus_.read(pc_++);
    if (execBranch(opcode))
        return;
    if (execImplied(opcode))
        return;
    if (execGroup0(opcode))
        return;
    if (execGroup1(opcode))
        return;
    if (execGroup2(opcode))
        return;
}

void CPU::addrImmediate() {
    addr_ = pc_++;
}

void CPU::addrRelative() {
    std::int8_t offset = bus_.read(pc_++);
    addr_ = pc_ + offset;
}

void CPU::addrAbsolute() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    pc_ += 2;
}

void CPU::addrZeroPage() {
    addr_ = bus_.read(pc_++);
}

void CPU::addrIndirect() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    addr_ = bus_.read(addr_);
    pc_ += 2;
}

void CPU::addrAbsoluteX() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8 + x_;
    pc_ += 2;
}

void CPU::addrAbsoluteY() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8 + y_;
    pc_ += 2;
}

void CPU::addrZeroPageX() {
    addr_ = (bus_.read(pc_++) + x_) & 0xFF;
}

void CPU::addrZeroPageY() {
    addr_ = (bus_.read(pc_++) + y_) & 0xFF;
}

void CPU::addrIndexedIndirect() {
    addr_ = bus_.read(pc_++) + x_;
    addr_ = bus_.read(addr_ & 0xFF) | bus_.read((addr_ + 1) & 0xFF) << 8;
}

void CPU::addrIndirectIndexed() {
    addr_ = bus_.read(pc_++);
    addr_ = bus_.read(addr_) | bus_.read((addr_ + 1) & 0xFF) << 8 + y_;
}

void CPU::setZ(std::uint8_t val) {
    p_.set(kZ, !val);
}

void CPU::setN(std::uint8_t val) {
    p_.set(kN, val & 0x80);
}

bool CPU::execBranch(std::uint8_t opcode) {
    if (opcode & 0x10) {
        bool condition = opcode & 0x20;
        switch ((opcode & 0xC0) >> 6) {
            case 0b00: // N
                condition = !(condition ^ p_.test(kN));
                break;
            case 0b01: // V
                condition = !(condition ^ p_.test(kV));
                break;
            case 0b10: // C
                condition = !(condition ^ p_.test(kC));
                break;
            case 0b11: // Z
                condition = !(condition ^ p_.test(kZ));
                break;
        }
        if (condition) {
            addrRelative();
            pc_ = addr_;
        }
        return true;
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
        switch ((opcode & 0x1C) >> 2) {
            case 0b000:
                addrImmediate();
                break;
            case 0b001:
                addrZeroPage();
                break;
            case 0b011:
                if (opcode == 0x6C) { // JMP (abs)
                    addrIndirect();
                } else {
                    addrAbsolute();
                }
                break;
            case 0b101:
                addrZeroPageX();
                break;
            case 0b111:
                addrAbsoluteX();
                break;
            default:
                return false;
        }
        switch ((opcode & 0xE0) >> 5) {
            case 0b001: // BIT
                break;
            case 0b010: // JMP
                break;
            case 0b011: // JMP (abs)
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
        std::uint16_t addr;
        switch ((opcode & 0x1C) >> 2) {
            case 0b000:
                addrIndexedIndirect();
                break;
            case 0b001:
                addrZeroPage();
                break;
            case 0b010:
                addrImmediate();
                break;
            case 0b011:
                addrAbsolute();
                break;
            case 0b100:
                addrIndirectIndexed();
                break;
            case 0b101:
                addrZeroPageX();
                break;
            case 0b110:
                addrAbsoluteY();
                break;
            case 0b111:
                addrAbsoluteX();
                break;
        }
        switch ((opcode & 0xE0) >> 5) {
            case 0b000: // ORA
                a_ |= bus_.read(addr_);
                setZ(a_);
                setN(a_);
                break;
            case 0b001: // AND
                a_ &= bus_.read(addr_);
                setZ(a_);
                setN(a_);
                break;
            case 0b010: // EOR
                a_ ^= bus_.read(addr_);
                setZ(a_);
                setN(a_);
                break;
            case 0b011: // ADC
                {
                    auto operand = bus_.read(addr_);
                    std::int16_t sum = a_ + operand + p_.test(kC);
                    p_.set(kC, sum & 0x100);
                    p_.set(kV, sum < -128 || sum > 127);
                    a_ = static_cast<std::uint8_t>(sum);
                    setZ(a_);
                    setN(a_);
                }
                break;
            case 0b100: // STA
                bus_.write(addr_, a_);
                break;
            case 0b101: // LDA
                a_ = bus_.read(addr_);
                setZ(a_);
                setN(a_);
                break;
            case 0b110: // CMP
                {
                    std::int16_t diff = a_ - bus_.read(addr_);
                    p_.set(kC, diff >= 0);
                    setZ(diff);
                    setN(diff);
                }
                break;
            case 0b111: // SBC
                {
                    std::int16_t diff = a_ - bus_.read(addr_) - p_.test(kC);
                    p_.set(kC, diff >= 0);
                    p_.set(kV, diff < -128 || diff > 127);
                    a_ = static_cast<std::uint8_t>(diff);
                    setZ(a_);
                    setN(a_);
                }
                break;
        }
        return true;
    }
    return false;
}

bool CPU::execGroup2(std::uint8_t opcode) {
    if ((opcode & 0x03) == 0x02) {
        auto mode = (opcode & 0x1C) >> 2;
        if (mode == 0b010) { // Accumulator
            switch ((opcode & 0xE0) >> 5) {
                case 0b000: // ASL
                    p_.set(kC, a_ & 0x80);
                    a_ <<= 1;
                    setZ(a_);
                    setN(a_);
                    break;
                case 0b001: // ROL
                    {
                        auto old_c = p_.test(kC);
                        p_.set(kC, a_ & 0x80);
                        a_ <<= 1;
                        a_ |= old_c;
                        setZ(a_);
                        setN(a_);
                    }
                    break;
                case 0b010: // LSR
                    p_.set(kC, a_ & 0x01);
                    a_ >>= 1;
                    setZ(a_);
                    p_.set(kN, false);
                    break;
                case 0b011: // ROR
                    {
                        auto old_c = p_.test(kC);
                        p_.set(kC, a_ & 0x01);
                        a_ >>= 1;
                        a_ |= old_c;
                        setZ(a_);
                        setN(a_);
                    }
                    break;
                default:
                    return false;
            }
        } else {
            switch (mode) {
                case 0b000:
                    addrImmediate();
                    break;
                case 0b001:
                    addrZeroPage();
                    break;
                case 0b011:
                    addrAbsolute();
                    break;
                case 0b101:
                    if (opcode == 0x96 || opcode == 0xB6) {
                        addrZeroPageY();
                    } else {
                        addrZeroPageX();
                    }
                    break;
                case 0b111:
                    if (opcode == 0xBE) { // LDX
                        addrAbsoluteY();
                    } else {
                        addrAbsoluteX();
                    }
                    break;
                default:
                    return false;
            }

            switch ((opcode & 0xE0) >> 5) {
                case 0b000: // ASL
                    {
                        auto operand = bus_.read(addr_);
                        p_.set(kC, operand & 0x80);
                        operand <<= 1;
                        setZ(operand);
                        setN(operand);
                    }
                    break;
                case 0b001: // ROL
                    {
                        auto operand = bus_.read(addr_);
                        auto old_c = p_.test(kC);
                        p_.set(kC, operand & 0x80);
                        operand <<= 1;
                        operand |= old_c;
                        setZ(operand);
                        setN(operand);
                    }
                    break;
                case 0b010: // LSR
                    {
                        auto operand = bus_.read(addr_);
                        p_.set(kC, operand & 0x01);
                        operand >>= 1;
                        setZ(operand);
                        p_.set(kN, false);
                    }
                    break;
                case 0b011: // ROR
                    {
                        auto operand = bus_.read(addr_);
                        auto old_c = p_.test(kC);
                        p_.set(kC, operand & 0x01);
                        operand >>= 1;
                        operand |= old_c;
                        bus_.write(addr_, operand);
                        setZ(operand);
                        setN(operand);
                    }
                    break;
                case 0b100: // STX
                    bus_.write(addr_, x_);
                    break;
                case 0b101: // LDX
                    x_ = bus_.read(addr_);
                    setZ(x_);
                    setN(x_);
                    break;
                case 0b110: // DEC
                    {
                        auto operand = bus_.read(addr_);
                        bus_.write(addr_, --operand);
                        setZ(operand);
                        setN(operand);
                    }
                    break;
                case 0b111: // INC
                    {
                        auto operand = bus_.read(addr_);
                        bus_.write(addr_, ++operand);
                        setZ(operand);
                        setN(operand);
                    }
                    break;
            }
        }
        return true;
    }
    return false;
}