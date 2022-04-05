#include "cpu.h"
using namespace nes;

Cpu::Cpu(Bus& bus) 
    : bus_(bus)
{

}

void Cpu::reset() {
    pc_ = bus_.read(0xFFFC) | bus_.read(0xFFFD) << 8;
    a_ = 0x00;
    x_ = 0x00;
    y_ = 0x00;
    s_ = 0xFD;
    p_.set(kI);
}

void Cpu::step() {
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

void Cpu::addrImmediate() {
    addr_ = pc_++;
}

void Cpu::addrRelative() {
    std::int8_t offset = bus_.read(pc_++);
    addr_ = pc_ + offset;
}

void Cpu::addrAbsolute() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    pc_ += 2;
}

void Cpu::addrZeroPage() {
    addr_ = bus_.read(pc_++);
}

void Cpu::addrIndirect() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    addr_ = bus_.read(addr_);
    pc_ += 2;
}

void Cpu::addrAbsoluteX() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8 + x_;
    pc_ += 2;
}

void Cpu::addrAbsoluteY() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8 + y_;
    pc_ += 2;
}

void Cpu::addrZeroPageX() {
    addr_ = (bus_.read(pc_++) + x_) & 0xFF;
}

void Cpu::addrZeroPageY() {
    addr_ = (bus_.read(pc_++) + y_) & 0xFF;
}

void Cpu::addrIndexedIndirect() {
    addr_ = bus_.read(pc_++) + x_;
    addr_ = bus_.read(addr_ & 0xFF) | bus_.read((addr_ + 1) & 0xFF) << 8;
}

void Cpu::addrIndirectIndexed() {
    addr_ = bus_.read(pc_++);
    addr_ = bus_.read(addr_) | bus_.read((addr_ + 1) & 0xFF) << 8 + y_;
}

void Cpu::setZ(std::uint8_t val) {
    p_.set(kZ, !val);
}

void Cpu::setN(std::uint8_t val) {
    p_.set(kN, val & 0x80);
}

void Cpu::push(std::uint8_t val) {
    bus_.write(0x0100 | s_--, val);
}

std::uint8_t Cpu::pull() {
    return bus_.read(0x0100 | s_++);
}

bool Cpu::execBranch(std::uint8_t opcode) {
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

bool Cpu::execImplied(std::uint8_t opcode) {
    switch (opcode) {
        case 0x00: // BRK
            ++pc_;
            push(pc_ >> 8);
            push(pc_);
            push(p_.to_ulong() | 1 << kB | 1 << 5);
            p_.set(kI);
            pc_ = bus_.read(0xFFFE) | bus_.read(0xFFFF) << 8;
            break;
        case 0x20: // JSR
            {
                push(pc_ + 1 >> 8);
                push(pc_ + 1);
                auto old_pc = pc_;
                pc_ = bus_.read(old_pc);
                pc_ |= bus_.read(old_pc + 1) << 8;
            }
            break;
        case 0x40: // RTI
            p_ = pull();
            pc_ = pull();
            pc_ |= pull() << 8;
            break;
        case 0x60: // RTS
            pc_ = pull();
            pc_ |= pull() << 8;
            ++pc_;
            break;
        case 0x08: // PHP
            push(p_.to_ulong() | 1 << 5);
            break;
        case 0x18: // CLC
            p_.reset(kC);
            break;
        case 0x28: // PLP
            p_ = pull();
            break;
        case 0x38: // SEC
            p_.set(kC);
            break;
        case 0x48: // PHA
            push(a_);
            break;
        case 0x58: // CLI
            p_.reset(kI);
            break;
        case 0x68: // PLA
            a_ = pull();
            setZ(a_);
            setN(a_);
            break;
        case 0x78: // SEI
            p_.set(kI);
            break;
        case 0x88: // DEY
            --y_;
            setZ(y_);
            setN(y_);
            break;
        case 0x98: // TYA
            a_ = y_;
            setZ(a_);
            setZ(a_);
            break;
        case 0xA8: // TAY
            y_ = a_;
            setZ(y_);
            setN(y_);
            break;
        case 0xB8: // CLV
            p_.reset(kV);
            break;
        case 0xC8: // INY
            ++y_;
            setZ(y_);
            setN(y_);
            break;
        case 0xD8: // CLD
            p_.reset(kD);
            break;
        case 0xE8: // INX
            ++x_;
            setZ(x_);
            setN(x_);
            break;
        case 0xF8: // SED
            p_.set(kD);
            break;
        case 0x8A: // TXA
            a_ = x_;
            setZ(a_);
            setN(a_);
            break;
        case 0x9A: // TXS
            s_ = x_;
            break;
        case 0xAA: // TAX
            x_ = a_;
            setZ(x_);
            setN(x_);
            break;
        case 0xBA: // TSX
            x_ = s_;
            setZ(x_);
            setN(x_);
            break;
        case 0xCA: // DEX
            --x_;
            setZ(x_);
            setN(x_);
            break;
        case 0xEA: // NOP
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execGroup0(std::uint8_t opcode) {
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
                {
                    auto operand = bus_.read(addr_);
                    p_.set(kN, operand & 0x80);
                    p_.set(kV, operand & 0x40);
                    p_.set(kZ, !(operand & a_));
                }
                break;
            case 0b010: // JMP
            case 0b011: // JMP (abs)
                pc_ = bus_.read(addr_);
                break;
            case 0b100: // STY
                bus_.write(addr_, y_);
                break;
            case 0b101: // LDY
                y_ = bus_.read(addr_);
                setZ(y_);
                setN(y_);
                break;
            case 0b110: // CPY
                {
                    std::int16_t diff = x_ - bus_.read(addr_);
                    p_.set(kC, !(diff & 0x100));
                    setZ(diff);
                    setN(diff);
                }
                break;
            case 0b111: // CPX
                {
                    std::int16_t diff = y_ - bus_.read(addr_);
                    p_.set(kC, !(diff & 0x100));
                    setZ(diff);
                    setN(diff);
                }
                break;
        }
        return true;
    }
    return false;
}

bool Cpu::execGroup1(std::uint8_t opcode) {
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
                    std::uint16_t sum = a_ + operand + p_.test(kC);
                    p_.set(kC, sum & 0x100);
                    p_.set(kV, ~(a_ ^ operand) & (a_ ^ sum) & 0x80);
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
                    p_.set(kC, !(diff & 0x100));
                    setZ(diff);
                    setN(diff);
                }
                break;
            case 0b111: // SBC
                {
                    auto operand = bus_.read(addr_);
                    std::uint16_t diff = a_ - operand - !p_.test(kC);
                    p_.set(kC, !(diff & 0x100));
                    p_.set(kV, (a_ ^ operand) & (a_ ^ diff) & 0x80);
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

bool Cpu::execGroup2(std::uint8_t opcode) {
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