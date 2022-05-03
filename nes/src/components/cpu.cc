#include "cpu.h"
using namespace nes;

#include <spdlog/spdlog.h>

#include "cpu_bus.h"

Cpu::Cpu(CpuBus& bus) 
    : bus_(bus)
{
    a_ = 0x00;
    x_ = 0x00;
    y_ = 0x00;
    pc_ = readAddress(kResetVector);
    s_ = 0xFD;
    p_ = 0x00;
    setI(true);
    cycle_ = 0;
}

void Cpu::irq() {
    if (!(p_ & kI)) {
        cycle_ += 2;
        push(pc_ >> 8);
        push(pc_);
        push((p_ | 1 << 5) & ~kB);
        setI(true);
        pc_ = readAddress(kIrqVector);
    }
}

void Cpu::nmi() {
    cycle_ += 2;
    push(pc_ >> 8);
    push(pc_);
    push((p_ | 1 << 5) & ~kB);
    setI(true);
    pc_ = readAddress(kNmiVector);
}

void Cpu::reset() {
    cycle_ += 5;
    pc_ = readAddress(kResetVector);
    s_ -= 0x03;
    setI(true);
}

void Cpu::cycle() {
    if (cycle_-- == 0) {
        auto opcode = readByte(pc_++);
        if (execOpcode(opcode))
            return;
        if (execImplied(opcode))
            return;
        if (execBranch(opcode))
            return;
        if (execGroup0(opcode))
            return;
        if (execGroup1(opcode))
            return;
        if (execGroup2(opcode))
            return;
        spdlog::critical("Invalid opcode {:#04x}", opcode);
    }
}

void Cpu::oamDma(std::uint8_t page) {
    if (cycle_ % 2 != 0) {
        ++cycle_;
    }

    addr_ = page << 8;
    for (int i = 0; i < 256; ++i) {
        std::uint8_t data = readByte(addr_);
        writeByte(0x2004, data);
        ++addr_;
    }
}

std::uint8_t Cpu::readByte(std::uint16_t addr) {
    ++cycle_;
    return bus_.read(addr);
}

std::uint16_t Cpu::readAddress(std::uint16_t addr) {
    return readByte(addr) | readByte(addr + 1) << 8;
}

void Cpu::writeByte(std::uint16_t addr, std::uint8_t val) {
    ++cycle_;
    bus_.write(addr, val);
}

std::uint8_t Cpu::pull() {
    return readByte(kStackBase | ++s_);
}

void Cpu::push(std::uint8_t val) {
    writeByte(kStackBase | s_--, val);
}

void Cpu::addrImmediate() {
    addr_ = pc_++;
}

void Cpu::addrRelative() {
    std::int8_t offset = readByte(pc_++);
    addr_ = pc_ + offset;
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
    addr_ = bus_.read(addr_) | bus_.read(addr_ & 0xFF00 | (addr_ + 1) & 0xFF) << 8;
}

void Cpu::addrAbsoluteX() {
    std::uint16_t base = readAddress(pc_);
    pc_ += 2;
    addr_ = base + x_;
    if ((base & 0xFF00) != (addr_ & 0xFF00)) {
        ++cycle_;
    }
}

void Cpu::addrAbsoluteY() {
    std::uint16_t base = readAddress(pc_);
    pc_ += 2;
    addr_ = base + y_;
    if ((base & 0xFF00) != (addr_ & 0xFF00)) {
        ++cycle_;
    }
}

void Cpu::addrZeroPageX() {
    addr_ = (readByte(pc_++) + x_) & 0xFF;
    ++cycle_;
}

void Cpu::addrZeroPageY() {
    addr_ = (readByte(pc_++) + y_) & 0xFF;
    ++cycle_;
}

void Cpu::addrIndexedIndirect() {
    addr_ = readByte(pc_++) + x_;
    ++cycle_;
    addr_ = readByte(addr_ & 0xFF) | readByte((addr_ + 1) & 0xFF) << 8;
}

void Cpu::addrIndirectIndexed() {
    addr_ = readByte(pc_++);
    std::uint16_t base = readByte(addr_) | readByte((addr_ + 1) & 0xFF) << 8;
    addr_ = base + y_;
    if ((base & 0xFF00) != (addr_ & 0xFF00)) {
        ++cycle_;
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

bool Cpu::execOpcode(std::uint8_t opcode) {
    switch (opcode) {
        case 0x20:
            push((pc_ + 1) >> 8);
            push(pc_ + 1);
            pc_ = readAddress(pc_);
            spdlog::debug("JSR abs");
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execImplied(std::uint8_t opcode) {
    switch (opcode) {
        case 0x00:
            ++pc_;
            push(pc_ >> 8);
            push(pc_);
            push(p_ | 1 << 5 | kB);
            setI(true);
            pc_ = readAddress(kBrkVector);
            spdlog::debug("BRK impl");
            break;
        case 0x40:
            p_ = pull() & 0xCF;
            pc_ = pull();
            pc_ |= pull() << 8;
            ++cycle_;
            spdlog::debug("RTI impl");
            break;
        case 0x60:
            pc_ = pull();
            pc_ |= pull() << 8;
            ++cycle_;
            ++pc_;
            ++cycle_;
            spdlog::debug("RTS impl");
            break;
        case 0x08:
            push(p_ | 1 << 5 | kB);
            spdlog::debug("PHP impl");
            break;
        case 0x18:
            setC(false);
            spdlog::debug("CLC impl");
            break;
        case 0x28:
            p_ = pull() & 0xCF;
            ++cycle_;
            spdlog::debug("PLP impl");
            break;
        case 0x38:
            setC(true);
            spdlog::debug("SEC impl");
            break;
        case 0x48:
            push(a_);
            spdlog::debug("PHA impl");
            break;
        case 0x58:
            setI(false);
            spdlog::debug("CLI impl");
            break;
        case 0x68:
            a_ = pull();
            ++cycle_;
            setZN(a_);
            spdlog::debug("PLA impl");
            break;
        case 0x78:
            setI(true);
            spdlog::debug("SEI impl");
            break;
        case 0x88:
            --y_;
            setZN(y_);
            spdlog::debug("DEY impl");
            break;
        case 0x98:
            a_ = y_;
            setZN(a_);
            spdlog::debug("TYA impl");
            break;
        case 0xA8:
            y_ = a_;
            setZN(y_);
            spdlog::debug("TAY impl");
            break;
        case 0xB8:
            setV(false);
            spdlog::debug("CLV impl");
            break;
        case 0xC8:
            ++y_;
            setZN(y_);
            spdlog::debug("INY impl");
            break;
        case 0xD8:
            setD(false);
            spdlog::debug("CLD impl");
            break;
        case 0xE8:
            ++x_;
            setZN(x_);
            spdlog::debug("INX impl");
            break;
        case 0xF8:
            setD(true);
            spdlog::debug("SED impl");
            break;
        case 0x8A:
            a_ = x_;
            setZN(a_);
            spdlog::debug("TXA impl");
            break;
        case 0x9A:
            s_ = x_;
            spdlog::debug("TXS impl");
            break;
        case 0xAA:
            x_ = a_;
            setZN(x_);
            spdlog::debug("TAX impl");
            break;
        case 0xBA:
            x_ = s_;
            setZN(x_);
            spdlog::debug("TSX impl");
            break;
        case 0xCA:
            --x_;
            setZN(x_);
            spdlog::debug("DEX impl");
            break;
        case 0xEA:
            spdlog::debug("NOP impl");
            break;
        default:
            return false;
    }
    ++cycle_;
    return true;
}

bool Cpu::execBranch(std::uint8_t opcode) {
    if ((opcode & 0x1F) == 0x10) {
        bool condition = opcode & 0x20;
        switch ((opcode & 0xC0) >> 6) {
            case 0b00:
                if (condition) {
                    condition = p_ & kN;
                    spdlog::debug("BMI rel");
                } else {
                    condition = !(p_ & kN);
                    spdlog::debug("BPL rel");
                }
                break;
            case 0b01:
                if (condition) {
                    condition = p_ & kV;
                    spdlog::debug("BVS rel");
                } else {
                    condition = !(p_ & kV);
                    spdlog::debug("BVC rel");
                }
                break;
            case 0b10:
                if (condition) {
                    condition = p_ & kC;
                    spdlog::debug("BCS rel");
                } else {
                    condition = !(p_ & kC);
                    spdlog::debug("BCC rel");
                }
                break;
            case 0b11:
                if (condition) {
                    condition = p_ & kZ;
                    spdlog::debug("BEQ rel");
                } else {
                    condition = !(p_ & kZ);
                    spdlog::debug("BNE rel");
                }
                break;
        }

        addrRelative();
        if (condition) {            
            if ((addr_ & 0xFF00) != (pc_ & 0xFF00)) {
                ++cycle_;
            }
            pc_ = addr_;
        }
        return true;
    }
    return false;
}

bool Cpu::execGroup0(std::uint8_t opcode) {
    std::string_view instr, mode;
    if ((opcode & 0x03) == 0x00) {
        switch ((opcode & 0x1C) >> 2) {
            case 0b000:
                addrImmediate();
                mode = "#";
                break;
            case 0b001:
                addrZeroPage();
                mode = "zpg";
                break;
            case 0b011:
                if (opcode == 0x6C) { // JMP (abs)
                    addrIndirect();
                    mode = "ind";
                } else {
                    addrAbsolute();
                    mode = "abs";
                }
                break;
            case 0b101:
                addrZeroPageX();
                mode = "zpg, X";
                break;
            case 0b111:
                addrAbsoluteX();
                mode = "abs, X";
                break;
            default:
                return false;
        }
        switch ((opcode & 0xE0) >> 5) {
            case 0b001:
                {
                    std::uint8_t operand = readByte(addr_);
                    setZ(!(operand & a_));
                    setV(operand & 0x40);
                    setN(operand & 0x80);
                }
                instr = "BIT";
                break;
            case 0b010:
            case 0b011:
                pc_ = addr_;
                instr = "JMP";
                break;
            case 0b100:
                writeByte(addr_, y_);
                instr = "STY";
                break;
            case 0b101:
                y_ = readByte(addr_);
                setZN(y_);
                instr = "LDY";
                break;
            case 0b110:
                {
                    std::uint16_t diff = y_ - readByte(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                instr = "CPY";
                break;
            case 0b111: // CPX
                {
                    std::uint16_t diff = x_ - readByte(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                instr = "CPX";
                break;
        }
        spdlog::debug("{} {}", instr, mode);
        return true;
    }
    return false;
}

bool Cpu::execGroup1(std::uint8_t opcode) {
    std::string_view instr, mode;
    if ((opcode & 0x03) == 0x01) {
        std::uint16_t addr;
        switch ((opcode & 0x1C) >> 2) {
            case 0b000:
                addrIndexedIndirect();
                mode = "X, ind";
                break;
            case 0b001:
                addrZeroPage();
                mode = "zpg";
                break;
            case 0b010:
                addrImmediate();
                mode = "#";
                break;
            case 0b011:
                addrAbsolute();
                mode = "abs";
                break;
            case 0b100:
                addrIndirectIndexed();
                mode = "ind, Y";
                break;
            case 0b101:
                addrZeroPageX();
                mode = "zpg, X";
                break;
            case 0b110:
                addrAbsoluteY();
                mode = "abs, Y";
                break;
            case 0b111:
                addrAbsoluteX();
                mode = "abs, X";
                break;
        }
        switch ((opcode & 0xE0) >> 5) {
            case 0b000:
                a_ |= readByte(addr_);
                setZN(a_);
                instr = "ORA";
                break;
            case 0b001:
                a_ &= readByte(addr_);
                setZN(a_);
                instr = "AND";
                break;
            case 0b010:
                a_ ^= readByte(addr_);
                setZN(a_);
                instr = "EOR";
                break;
            case 0b011:
                {
                    std::uint8_t operand = readByte(addr_);
                    std::uint16_t sum = a_ + operand + (p_ & kC);
                    setC(sum & 0x100);
                    setV(~(a_ ^ operand) & (a_ ^ sum) & 0x80);
                    a_ = static_cast<std::uint8_t>(sum);
                }
                setZN(a_);
                instr = "ADC";
                break;
            case 0b100:
                writeByte(addr_, a_);
                instr = "STA";
                break;
            case 0b101:
                a_ = readByte(addr_);
                setZN(a_);
                instr = "LDA";
                break;
            case 0b110:
                {
                    std::uint16_t diff = a_ - readByte(addr_);
                    setC(!(diff & 0x100));
                    setZN(diff);
                }
                instr = "CMP";
                break;
            case 0b111:
                {
                    std::uint8_t operand = readByte(addr_);
                    std::uint16_t diff = a_ - operand - !(p_ & kC);
                    setC(!(diff & 0x100));
                    setV((a_ ^ operand) & (a_ ^ diff) & 0x80);
                    a_ = static_cast<std::uint8_t>(diff);
                }
                setZN(a_);
                instr = "SBC";
                break;
        }
        spdlog::debug("{} {}", instr, mode);
        return true;
    }
    return false;
}

bool Cpu::execGroup2(std::uint8_t opcode) {
    std::string_view instr, mode;
    if ((opcode & 0x03) == 0x02) {
        auto mode_flag = (opcode & 0x1C) >> 2;
        if (mode_flag == 0b010) {
            mode = "A";
            switch ((opcode & 0xE0) >> 5) {
                case 0b000:
                    setC(a_ & 0x80);
                    a_ <<= 1;
                    setZN(a_);
                    instr = "ASL";
                    break;
                case 0b001:
                    {
                        bool old_c = p_ & kC;
                        setC(a_ & 0x80);
                        a_ <<= 1;
                        a_ |= old_c;
                    }
                    setZN(a_);
                    instr = "ROL";
                    break;
                case 0b010:
                    setC(a_ & 0x01);
                    a_ >>= 1;
                    setZN(a_);
                    instr = "LSR";
                    break;
                case 0b011:
                    {
                        bool old_c = p_ & kC;
                        setC(a_ & 0x01);
                        a_ >>= 1;
                        a_ |= old_c << 7;
                    }
                    setZN(a_);
                    instr = "ROR";
                    break;
                default:
                    return false;
            }
        } else {
            switch (mode_flag) {
                case 0b000:
                    addrImmediate();
                    mode = "#";
                    break;
                case 0b001:
                    addrZeroPage();
                    mode = "zpg";
                    break;
                case 0b011:
                    addrAbsolute();
                    mode = "abs";                    
                    break;
                case 0b101:
                    if (opcode == 0x96 || opcode == 0xB6) {
                        addrZeroPageY();
                        mode = "zpg, Y";
                    } else {
                        addrZeroPageX();
                        mode = "zpg, X";
                    }
                    break;
                case 0b111:
                    if (opcode == 0xBE) { // LDX
                        addrAbsoluteY();
                        mode = "abs, Y";
                    } else {
                        addrAbsoluteX();
                        mode = "abs, X";
                    }
                    break;
                default:
                    return false;
            }

            switch ((opcode & 0xE0) >> 5) {
                case 0b000:
                    {
                        std::uint8_t operand = readByte(addr_);
                        setC(operand & 0x80);
                        operand <<= 1;
                        ++cycle_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    instr = "ASL";
                    break;
                case 0b001:
                    {
                        std::uint8_t operand = readByte(addr_);
                        bool old_c = p_ & kC;
                        setC(operand & 0x80);
                        operand <<= 1;
                        ++cycle_;
                        operand |= old_c;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    instr = "ROL";
                    break;
                case 0b010:
                    {
                        auto operand = readByte(addr_);
                        setC(operand & 0x01);
                        operand >>= 1;
                        ++cycle_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    instr = "LSR";
                    break;
                case 0b011:
                    {
                        std::uint8_t operand = readByte(addr_);
                        bool old_c = p_ & kC;
                        setC(operand & 0x01);
                        operand >>= 1;
                        ++cycle_;
                        operand |= old_c << 7;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    instr = "ROR";
                    break;
                case 0b100:
                    writeByte(addr_, x_);
                    instr = "STX";
                    break;
                case 0b101:
                    x_ = readByte(addr_);
                    setZN(x_);
                    instr = "LDX";
                    break;
                case 0b110:
                    {
                        std::uint8_t operand = readByte(addr_);
                        --operand;
                        ++cycle_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    instr = "DEC";
                    break;
                case 0b111:
                    {
                        std::uint8_t operand = readByte(addr_);
                        ++operand;
                        ++cycle_;
                        writeByte(addr_, operand);
                        setZN(operand);
                    }
                    instr = "INC";
                    break;
            }
        }
        spdlog::debug("{} {}", instr, mode);
        return true;
    }
    return false;
}