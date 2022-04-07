#include "cpu.h"
using namespace nes;

Cpu::Cpu(Bus& bus) 
    : bus_(bus)
{

}

void Cpu::irq() {
    if (!(p_ & kI)) {
        push(pc_ >> 8);
        push(pc_);
        push(p_ | 1 << 5 & ~kB);
        setI(true);
        pc_ = bus_.read(kIrqVector) | bus_.read(kIrqVector + 1) << 8;   
    }
}

void Cpu::nmi() {
    push(pc_ >> 8);
    push(pc_);
    push(p_ | 1 << 5 & ~kB);
    setI(true);
    pc_ = bus_.read(kNmiVector) | bus_.read(kNmiVector + 1) << 8;   
}

void Cpu::reset() {
    pc_ = bus_.read(kResetVector) | bus_.read(kResetVector + 1) << 8;
    a_ = 0x00;
    x_ = 0x00;
    y_ = 0x00;
    s_ = 0xFD;
    setI(true);
}

int Cpu::step() {
    cycles_ = 2;
    auto opcode = bus_.read(pc_++);
    if (execBranch(opcode))
        return cycles_;
    if (execImplied(opcode))
        return cycles_;
    if (execGroup0(opcode))
        return cycles_;
    if (execGroup1(opcode))
        return cycles_;
    if (execGroup2(opcode))
        return cycles_;
    return cycles_;
}

void Cpu::addrImmediate() {
    addr_ = pc_++;
}

void Cpu::addrRelative() {
    std::int8_t offset = bus_.read(pc_++);
    addr_ = pc_ + offset;
    cycles_ += 1;
}

void Cpu::addrAbsolute() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    pc_ += 2;
    cycles_ += 2;
}

void Cpu::addrZeroPage() {
    addr_ = bus_.read(pc_++);
    cycles_ += 1;
}

void Cpu::addrIndirect() {
    addr_ = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    addr_ = bus_.read(addr_) | bus_.read(addr_ + 1) << 8;
    pc_ += 2;
    cycles_ += 4;
}

void Cpu::addrAbsoluteX() {
    std::uint16_t base = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    addr_ = base + x_;
    pc_ += 2;
    cycles_ += 2;
    if (base & 0xFF00 != addr_ & 0xFF00) {
        ++cycles_;
    }
}

void Cpu::addrAbsoluteY() {
    std::uint16_t base = bus_.read(pc_) | bus_.read(pc_ + 1) << 8;
    addr_ = base + y_;
    pc_ += 2;
    cycles_ += 2;
    if (base & 0xFF00 != addr_ & 0xFF00) {
        ++cycles_;
    }
}

void Cpu::addrZeroPageX() {
    addr_ = (bus_.read(pc_++) + x_) & 0xFF;
    cycles_ += 2;
}

void Cpu::addrZeroPageY() {
    addr_ = (bus_.read(pc_++) + y_) & 0xFF;
    cycles_ += 2;
}

void Cpu::addrIndexedIndirect() {
    addr_ = bus_.read(pc_++) + x_;
    addr_ = bus_.read(addr_ & 0xFF) | bus_.read((addr_ + 1) & 0xFF) << 8;
    cycles_ += 4;
}

void Cpu::addrIndirectIndexed() {
    addr_ = bus_.read(pc_++);
    std::uint16_t base = bus_.read(addr_) | bus_.read((addr_ + 1) & 0xFF) << 8;
    addr_ = base + y_;
    cycles_ += 3;
    if (base & 0xFF00 != addr_ & 0xFF00) {
        ++cycles_;
    }
}

void Cpu::setC(bool val) {
    if (val) {
        p_ |= kC;
    } else {
        p_ &= ~kC;
    }
}

void Cpu::setZ(bool val) {
    if (val) {
        p_ |= kZ;
    } else {
        p_ &= ~kZ;
    }
}

void Cpu::setI(bool val) {
    if (val) {
        p_ |= kI;
    } else {
        p_ &= ~kI;
    }
}

void Cpu::setD(bool val) {
    if (val) {
        p_ |= kD;
    } else {
        p_ &= ~kD;
    }
}

void Cpu::setN(bool val) {
    if (val) {
        p_ |= kN;
    } else {
        p_ &= ~kN;
    }
}

void Cpu::setV(bool val) {
    if (val) {
        p_ |= kV;
    } else {
        p_ &= ~kV;
    }
}

void Cpu::setZN(std::uint8_t val) {
    setZ(!val);
    setN(val & 0x80);
}

void Cpu::push(std::uint8_t val) {
    bus_.write(kStackBase | s_--, val);
}

std::uint8_t Cpu::pull() {
    return bus_.read(kStackBase | s_++);
}

bool Cpu::execBranch(std::uint8_t opcode) {
    if (opcode & 0x10) {
        bool condition = opcode & 0x20;
        switch ((opcode & 0xC0) >> 6) {
            case 0b00:
                condition = !(condition ^ p_ & kN);
                break;
            case 0b01:
                condition = !(condition ^ p_ & kV);
                break;
            case 0b10:
                condition = !(condition ^ p_ & kC);
                break;
            case 0b11:
                condition = !(condition ^ p_ & kZ);
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
            push(p_ | 1 << 5 | kB);
            setI(true);
            pc_ = bus_.read(kBrkVector) | bus_.read(kBrkVector + 1) << 8;
            break;
        case 0x20: // JSR
            {
                push(pc_ + 1 >> 8);
                push(pc_ + 1);
                auto old_pc = pc_;
                pc_ = bus_.read(old_pc);
                pc_ |= bus_.read(old_pc + 1) << 8;
                cycles_ += 4;
            }
            break;
        case 0x40: // RTI
            p_ = pull();
            pc_ = pull();
            pc_ |= pull() << 8;
            cycles_ += 4;
            break;
        case 0x60: // RTS
            pc_ = pull();
            pc_ |= pull() << 8;
            ++pc_;
            cycles_ += 4;
            break;
        case 0x08: // PHP
            push(p_ | 1 << 5 | kB);
            break;
        case 0x18: // CLC
            setC(false);
            break;
        case 0x28: // PLP
            p_ = pull();
            cycles_ += 2;
            break;
        case 0x38: // SEC
            setC(true);
            break;
        case 0x48: // PHA
            push(a_);
            break;
        case 0x58: // CLI
            setI(false);
            break;
        case 0x68: // PLA
            a_ = pull();
            setZ(a_);
            setN(a_);
            cycles_ += 2;
            break;
        case 0x78: // SEI
            setI(true);
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
            setV(false);
            break;
        case 0xC8: // INY
            ++y_;
            setZ(y_);
            setN(y_);
            break;
        case 0xD8: // CLD
            setD(false);
            break;
        case 0xE8: // INX
            ++x_;
            setZ(x_);
            setN(x_);
            break;
        case 0xF8: // SED
            setD(true);
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
                    setZ(!(operand & a_));
                    setV(operand & 0x40);
                    setN(operand & 0x80);
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
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                break;
            case 0b111: // CPX
                {
                    std::int16_t diff = y_ - bus_.read(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
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
                setZN(a_);
                break;
            case 0b001: // AND
                a_ &= bus_.read(addr_);
                setZN(a_);
                break;
            case 0b010: // EOR
                a_ ^= bus_.read(addr_);
                setZN(a_);
                break;
            case 0b011: // ADC
                {
                    auto operand = bus_.read(addr_);
                    std::uint16_t sum = a_ + operand + (p_ & kC);
                    setC(sum & 0x100);
                    setV(~(a_ ^ operand) & (a_ ^ sum) & 0x80);
                    a_ = static_cast<std::uint8_t>(sum);
                    setZN(a_);
                }
                break;
            case 0b100: // STA
                bus_.write(addr_, a_);
                break;
            case 0b101: // LDA
                a_ = bus_.read(addr_);
                setZN(a_);
                break;
            case 0b110: // CMP
                {
                    std::int16_t diff = a_ - bus_.read(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                break;
            case 0b111: // SBC
                {
                    auto operand = bus_.read(addr_);
                    std::uint16_t diff = a_ - operand - !(p_ & kC);
                    setC(!(diff & 0x100));
                    setV((a_ ^ operand) & (a_ ^ diff) & 0x80);
                    a_ = static_cast<std::uint8_t>(diff);
                    setZN(a_);
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
                    setC(a_ & 0x80);
                    a_ <<= 1;
                    setZN(a_);
                    break;
                case 0b001: // ROL
                    {
                        auto old_c = p_ & kC;
                        setC(a_ & 0x80);
                        a_ <<= 1;
                        a_ |= old_c;
                        setZN(a_);
                    }
                    break;
                case 0b010: // LSR
                    setC(a_ & 0x01);
                    a_ >>= 1;
                    setZ(!a_);
                    setN(false);
                    break;
                case 0b011: // ROR
                    {
                        auto old_c = p_ & kC;
                        setC(a_ & 0x01);
                        a_ >>= 1;
                        a_ |= old_c;
                        setZN(a_);
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
                        setC(operand & 0x80);
                        operand <<= 1;
                        bus_.write(addr_, operand);
                        setZN(operand);
                        cycles_ += 2;
                    }
                    break;
                case 0b001: // ROL
                    {
                        auto operand = bus_.read(addr_);
                        auto old_c = p_ & kC;
                        setC(operand & 0x80);
                        operand <<= 1;
                        operand |= old_c;
                        bus_.write(addr_, operand);
                        setZN(operand);
                        cycles_ += 2;
                    }
                    break;
                case 0b010: // LSR
                    {
                        auto operand = bus_.read(addr_);
                        setC(operand & 0x01);
                        operand >>= 1;
                        bus_.write(addr_, operand);
                        setZ(!operand);
                        setN(false);
                        cycles_ += 2;
                    }
                    break;
                case 0b011: // ROR
                    {
                        auto operand = bus_.read(addr_);
                        auto old_c = p_ & kC;
                        setC(operand & 0x01);
                        operand >>= 1;
                        operand |= old_c;
                        bus_.write(addr_, operand);
                        setZN(operand);
                        cycles_ += 2;
                    }
                    break;
                case 0b100: // STX
                    bus_.write(addr_, x_);
                    break;
                case 0b101: // LDX
                    x_ = bus_.read(addr_);
                    setZN(x_);
                    break;
                case 0b110: // DEC
                    {
                        auto operand = bus_.read(addr_);
                        bus_.write(addr_, --operand);
                        setZN(operand);
                        cycles_ += 2;
                    }
                    break;
                case 0b111: // INC
                    {
                        auto operand = bus_.read(addr_);
                        bus_.write(addr_, ++operand);
                        setZN(operand);
                        cycles_ += 2;
                    }
                    break;
            }
        }
        return true;
    }
    return false;
}