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
        pc_ = readAddress(kIrqVector);
    }
}

void Cpu::nmi() {
    push(pc_ >> 8);
    push(pc_);
    push(p_ | 1 << 5 & ~kB);
    setI(true);
    pc_ = readAddress(kNmiVector);   
}

void Cpu::reset() {
    setI(true);
    pc_ = readAddress(kResetVector);
    a_ = 0x00;
    x_ = 0x00;
    y_ = 0x00;
    s_ = 0xFD;
}

int Cpu::step() {
    cycles_ = 0;
    auto opcode = readByte(pc_++);
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

std::uint8_t Cpu::readByte(std::uint16_t addr) {
    ++cycles_;
    return bus_.read(addr);
}

void Cpu::writeByte(std::uint16_t addr, std::uint8_t val) {
    ++cycles_;
    bus_.write(addr, val);
}

std::uint16_t Cpu::readAddress(std::uint16_t addr) {
    return readByte(addr) | readByte(addr + 1) << 8;
}

void Cpu::push(std::uint8_t val) {
    writeByte(kStackBase | s_--, val);
}

std::uint8_t Cpu::pull() {
    return readByte(kStackBase | s_++);
}

void Cpu::addrImmediate() {
    addr_ = pc_++;
}

void Cpu::addrRelative() {
    std::int8_t offset = readByte(pc_++);
    addr_ = pc_ + offset;
    if (addr_ & 0xFF00 != pc_ & 0xFF00) {
        ++cycles_;
    }
}

void Cpu::addrAbsolute() {
    addr_ = readAddress(pc_);
    pc_ += 2;
}

void Cpu::addrZeroPage() {
    addr_ = readByte(pc_++);
}

void Cpu::addrIndirect() {
    addr_ = readAddress(pc_);
    pc_ += 2;
    addr_ = readAddress(addr_);
}

void Cpu::addrAbsoluteX() {
    std::uint16_t base = readAddress(pc_);
    pc_ += 2;
    addr_ = base + x_;
    if (base & 0xFF00 != addr_ & 0xFF00) {
        ++cycles_;
    }
}

void Cpu::addrAbsoluteY() {
    std::uint16_t base = readAddress(pc_);
    pc_ += 2;
    addr_ = base + y_;
    if (base & 0xFF00 != addr_ & 0xFF00) {
        ++cycles_;
    }
}

void Cpu::addrZeroPageX() {
    addr_ = (readByte(pc_++) + x_) & 0xFF;
    ++cycles_;
}

void Cpu::addrZeroPageY() {
    addr_ = (readByte(pc_++) + y_) & 0xFF;
    ++cycles_;
}

void Cpu::addrIndexedIndirect() {
    addr_ = readByte(pc_++) + x_;
    ++cycles_;
    addr_ = readByte(addr_) | readByte((addr_ + 1) & 0xFF) << 8;
}

void Cpu::addrIndirectIndexed() {
    addr_ = readByte(pc_++);
    std::uint16_t base = readByte(addr_) | readByte((addr_ + 1) & 0xFF) << 8;
    addr_ = base + y_;
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
        } else {
            ++cycles_;
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
            pc_ = readAddress(kBrkVector);
            break;
        case 0x20: // JSR
            {
                push(pc_ + 1 >> 8);
                push(pc_ + 1);
                pc_ = readAddress(pc_);
            }
            break;
        case 0x40: // RTI
            p_ = pull();
            pc_ = pull();
            pc_ |= pull() << 8;
            ++cycles_;
            break;
        case 0x60: // RTS
            pc_ = pull();
            pc_ |= pull() << 8;
            ++cycles_;
            ++pc_;
            ++cycles_;
            break;
        case 0x08: // PHP
            push(p_ | 1 << 5 | kB);
            break;
        case 0x18: // CLC
            setC(false);
            break;
        case 0x28: // PLP
            p_ = pull();
            ++cycles_;
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
            ++cycles_;
            setZ(a_);
            setN(a_);
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
    if (opcode != 0x20) { // JSR
        ++cycles_;
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
                    auto operand = readByte(addr_);
                    setZ(!(operand & a_));
                    setV(operand & 0x40);
                    setN(operand & 0x80);
                }
                break;
            case 0b010: // JMP
            case 0b011: // JMP (abs)
                pc_ = addr_;
                break;
            case 0b100: // STY
                writeByte(addr_, y_);
                break;
            case 0b101: // LDY
                y_ = readByte(addr_);
                setZ(y_);
                setN(y_);
                break;
            case 0b110: // CPY
                {
                    std::int16_t diff = x_ - readByte(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                break;
            case 0b111: // CPX
                {
                    std::int16_t diff = y_ - readByte(addr_);
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
                a_ |= readByte(addr_);
                setZN(a_);
                break;
            case 0b001: // AND
                a_ &= readByte(addr_);
                setZN(a_);
                break;
            case 0b010: // EOR
                a_ ^= readByte(addr_);
                setZN(a_);
                break;
            case 0b011: // ADC
                {
                    auto operand = readByte(addr_);
                    std::uint16_t sum = a_ + operand + (p_ & kC);
                    setC(sum & 0x100);
                    setV(~(a_ ^ operand) & (a_ ^ sum) & 0x80);
                    a_ = static_cast<std::uint8_t>(sum);
                    setZN(a_);
                }
                break;
            case 0b100: // STA
                writeByte(addr_, a_);
                break;
            case 0b101: // LDA
                a_ = readByte(addr_);
                setZN(a_);
                break;
            case 0b110: // CMP
                {
                    std::int16_t diff = a_ - readByte(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                break;
            case 0b111: // SBC
                {
                    auto operand = readByte(addr_);
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
                        auto operand = readByte(addr_);
                        setC(operand & 0x80);
                        operand <<= 1;
                        ++cycles_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    break;
                case 0b001: // ROL
                    {
                        auto operand = readByte(addr_);
                        auto old_c = p_ & kC;
                        setC(operand & 0x80);
                        operand <<= 1;
                        ++cycles_;
                        operand |= old_c;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    break;
                case 0b010: // LSR
                    {
                        auto operand = readByte(addr_);
                        setC(operand & 0x01);
                        operand >>= 1;
                        ++cycles_;
                        writeByte(addr_, operand);
                        setZ(!operand);
                        setN(false);
                    }
                    break;
                case 0b011: // ROR
                    {
                        auto operand = readByte(addr_);
                        auto old_c = p_ & kC;
                        setC(operand & 0x01);
                        operand >>= 1;
                        ++cycles_;
                        operand |= old_c;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    break;
                case 0b100: // STX
                    writeByte(addr_, x_);
                    break;
                case 0b101: // LDX
                    x_ = readByte(addr_);
                    setZN(x_);
                    break;
                case 0b110: // DEC
                    {
                        auto operand = readByte(addr_);
                        --operand;
                        ++cycles_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    break;
                case 0b111: // INC
                    {
                        auto operand = readByte(addr_);
                        ++operand;
                        ++cycles_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    break;
            }
        }
        return true;
    }
    return false;
}