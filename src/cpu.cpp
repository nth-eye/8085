#include "cpu.h"

namespace intel_8085 {

// SECTION Instructions

// NOTE Stack, I/O and Machine Control

void CPU::hlt() 
{
    halted = true;
}

void CPU::di() 
{
    IE = false;
}

void CPU::ei() 
{
    IE = true;
}

void CPU::rim() 
{
    PS.A = 0;
    PS.A |= M55;
    PS.A |= M65 << 1;
    PS.A |= M75 << 2;
    PS.A |= IE << 3;
    PS.A |= RST55 << 4;
    PS.A |= RST65 << 5;
    PS.A |= RST75_FF << 6;
    PS.A |= SID << 7;
}

void CPU::sim() 
{
    // SDE - Serial data enable - set to 1 for sending
    if (PS.A & 0b01000000)
        SOD = PS.A & 0b10000000;

    // MSE - Mask set enable - set to 1 to mask interrupts
    if (PS.A & 0b00000100) {
        M75 = PS.A & 0b00000100;
        M65 = PS.A & 0b00000010;
        M55 = PS.A & 0b00000001;
    }
    // Reset RTS 7.5 flip-flop
    if (PS.A & 0b00010000)
        RST75_FF = false;
}

void CPU::out()
{
    // Byte addr = fetch_byte();
    // port[addr] = PS.A;
}

void CPU::in()
{
    // Byte addr = fetch_byte();
    // PS.A = read_port(addr);
}

void CPU::xthl()
{
    std::swap(HL._0, mem[SP.word]);
    std::swap(HL._1, mem[SP.word + 1]);
}

void CPU::sphl()
{
    SP = HL;
}

void CPU::pop(RegPair &reg)
{
    reg._0 = mem[SP++];
    reg._1 = mem[SP++];
}

void CPU::push(RegPair &reg)
{
    mem[--SP] = reg._1;
    mem[--SP] = reg._0;
}

// NOTE Branching instructions

void CPU::pchl() 
{
    PC = HL.word;
}

void CPU::rst(Word addr) 
{
    mem[--SP] = PC >> 8;
    mem[--SP] = PC & 0xFF;
    PC = addr;
}

void CPU::jmp(bool flag) 
{
    Word tmp = fetch_word();
    PC = flag ? tmp : PC;
}

void CPU::call(bool flag) 
{
    if (flag) {
        Word tmp = fetch_word();
        mem[--SP] = PC >> 8;
        mem[--SP] = PC & 0xFF;
        PC = tmp;
    }
}

void CPU::ret(bool flag) 
{
    PC = mem[SP++];
    PC |= Word(mem[SP++]) << 8;
}

// NOTE Data transfer instructions

void CPU::xchg()
{
    std::swap(HL, DE);
}

void CPU::ldax(RegPair &reg)
{
    PS.A = mem[reg.word];
}

void CPU::lhld()
{
    Word addr = fetch_word();
    HL._0 = mem[addr++];
    HL._1 = mem[addr];
}

void CPU::lda()
{
    Word addr = fetch_word();
    PS.A = mem[addr];
}

void CPU::stax(RegPair &reg)
{
    mem[reg.word] = PS.A;
}

void CPU::shld()
{
    Word addr = fetch_word();
    mem[addr++] = HL._0;
    mem[addr]   = HL._1;
}

void CPU::sta()
{
    Word addr = fetch_word();
    mem[addr] = PS.A;
}

void CPU::lxi(RegPair &reg)
{
    reg._0 = fetch_byte();
    reg._1 = fetch_byte();
}

void CPU::mvi(Byte &dst)
{
    dst = fetch_byte();
}

void CPU::mov(Byte &dst, Byte &src)
{
    dst = src;
}

// NOTE Arithmetic instructions

void CPU::daa()
{
    Byte tmp = 0;
    Byte act = PS.A;

    if ((PS.A & 0x0F) > 0x09 || PS.AC)
        PS.A += (tmp |= 0x06);

    if ((PS.A & 0xF0) > 0x90 || PS.CY) {
        PS.A += 0x60;
        tmp |= 0x60;
    }

    set_aux_carry(act, tmp);
    set_carry(act, tmp);
    set_pzs(PS.A);
}

void CPU::adi()
{
    Byte tmp = fetch_byte();
    set_carry(PS.A, tmp);
    set_aux_carry(PS.A, tmp);
    PS.A += tmp;
    set_pzs(PS.A);
}

void CPU::aci()
{
    Byte tmp = fetch_byte() + PS.CY;
    set_carry(PS.A, tmp);
    set_aux_carry(PS.A, tmp);
    PS.A += tmp;
    set_pzs(PS.A);
}

void CPU::sui()
{
    Byte tmp = fetch_byte();
    set_carry(PS.A, -tmp);
    set_aux_carry(PS.A, -tmp);
    PS.A -= tmp;
    set_pzs(PS.A);
    cmc();
}

void CPU::sbi()
{
    Byte tmp = fetch_byte() + PS.CY;
    set_carry(PS.A, -tmp);
    set_aux_carry(PS.A, -tmp);
    PS.A -= tmp;
    set_pzs(PS.A);
    cmc();
}

void CPU::dad(RegPair &reg)
{
    Word tmp = HL.word;
    HL += reg;
    PS.CY = HL.word < tmp ? 1 : 0;
}

void CPU::inx(RegPair &reg)
{
    ++reg;
}

void CPU::dcx(RegPair &reg)
{
    --reg;
}

void CPU::inr(Byte &reg)
{
    set_aux_carry(reg, 1);
    set_pzs(++reg);
}

void CPU::dcr(Byte &reg)
{
    set_aux_carry(reg, -1);
    set_pzs(--reg);
}

void CPU::add(Byte &reg)
{
    Byte tmp = reg;
    set_carry(PS.A, tmp);
    set_aux_carry(PS.A, tmp);
    PS.A += tmp;
    set_pzs(PS.A);
}

void CPU::adc(Byte &reg)
{
    Byte tmp = reg + PS.CY;
    set_carry(PS.A, tmp);
    set_aux_carry(PS.A, tmp);
    PS.A += tmp;
    set_pzs(PS.A);
}

void CPU::sub(Byte &reg)
{
    Byte tmp = reg;
    set_carry(PS.A, -tmp);
    set_aux_carry(PS.A, -tmp);
    PS.A -= tmp;
    set_pzs(PS.A);
    cmc();
}

void CPU::sbb(Byte &reg)
{
    Byte tmp = reg + PS.CY;
    set_carry(PS.A, -tmp);
    set_aux_carry(PS.A, -tmp);
    PS.A -= tmp;
    set_pzs(PS.A);
    cmc();
}

// NOTE Logical instructions

void CPU::cma()
{
    PS.A = ~PS.A;
}

void CPU::stc() 
{
    PS.CY = 1;
}

void CPU::cmc() 
{
    PS.CY = ~PS.CY;
}

void CPU::rlc()
{
    PS.CY = PS.A & 0b10000000 ? 1 : 0;
    PS.A <<= 1;
    PS.A |= PS.CY;
}

void CPU::rrc()
{
    PS.CY = PS.A & 0b00000001 ? 1 : 0;
    PS.A >>= 1;
    PS.A |= PS.CY << 7;
}

void CPU::ral()
{
    Byte tmp = PS.A & 0b10000000;
    PS.A <<= 1;
    PS.A |= PS.CY;
    PS.CY = tmp ? 1 : 0;
}

void CPU::rar()
{
    Byte tmp = PS.A & 0b00000001;
    PS.A >>= 1;
    PS.A |= PS.CY << 7;
    PS.CY = tmp ? 1 : 0;
}

void CPU::ani()
{
    PS.A &= fetch_byte();
    PS.CY = 0;
    PS.AC = 1;
    set_pzs(PS.A);
}

void CPU::xri()
{
    PS.A ^= fetch_byte();
    PS.CY = 0;
    PS.AC = 0;
    set_pzs(PS.A);
}

void CPU::ori()
{
    PS.A |= fetch_byte();
    PS.CY = 0;
    PS.AC = 0;
    set_pzs(PS.A);
}

void CPU::cpi() 
{
    Byte tmp = fetch_byte();
    PS.CY = PS.A < tmp ? 1 : 0;
    set_aux_carry(PS.A, -tmp);
    set_pzs(PS.A - tmp);
}

void CPU::ana(Byte &reg)
{
    PS.A &= reg;
    PS.CY = 0;
    PS.AC = 1;
    set_pzs(PS.A);
}

void CPU::xra(Byte &reg)
{
    PS.A ^= reg;
    PS.CY = 0;
    PS.AC = 0;
    set_pzs(PS.A);
}

void CPU::ora(Byte &reg)
{
    PS.A |= reg;
    PS.CY = 0;
    PS.AC = 0;
    set_pzs(PS.A);
}

void CPU::cmp(Byte &reg) 
{
    PS.CY = PS.A < reg ? 1 : 0;
    set_aux_carry(PS.A, -reg);
    set_pzs(PS.A - reg);
}

// !SECTION Instructions




// SECTION Main functions

Word CPU::fetch_word()          
{ 
    Word lsb = fetch_byte();
    Word msb = fetch_byte();
    return lsb | (msb << 8); 
}
Byte CPU::fetch_byte()          { return mem[PC++];  }
// Byte CPU::read_port(Byte addr)  { return port[addr]; }

void CPU::set_pzs(Byte res)
{
    // PS.CY = not affected
    PS.P = std::popcount(res) & 1 ? 0 : 1;
     // PS.AC = not affected
    PS.Z = res == 0;
    PS.S = res & 0b10000000 ? 1 : 0;
}

void CPU::set_carry(Byte op_1, Byte op_2)
{
    Word res = op_1 + op_2;
    PS.CY = res & 0xFF00 ? 1 : 0;
}

void CPU::set_aux_carry(Byte op_1, Byte op_2)
{
    PS.AC = ((op_1 & 0xF) + (op_2 & 0xF)) & 0xF0 ? 1 : 0;
}

void CPU::reset()
{
    PC  = 0x1000;
    SP  = mem.size();

    PS  = 0;

    BC  = 0;
    DE  = 0;
    HL  = 0;

    IR  = 0;

    cycles = 0;

    mem.clear();
}

void CPU::reset(Memory &mem_)
{
    auto size = mem_.size();

    if (size < 256 || size > 65536) {
        return;
        sout << "Inappropriate memory size: " << size 
             << "\nMinimum is 256" << "\nMaximum is 65536";
    }

    mem = mem_;

    reset();
}

void CPU::exec(size_t num_ins)
{
    for (size_t i = 0; i < num_ins; ++i) {

        if (halted) {
            sout << "Processor is halted";
            return;
        }

        // if (PC == 0x101A) {
        //     for (Word i = 0x0018; i < 0x0018 + 12; ++i)
        //         std::cout << std::hex << int(mem[i]) << ' ';
        //     sout << ' ';
        // }

        IR = fetch_byte();

#define M mem[HL.word]
#define CASE(ins, fn, ...) case ins: fn(__VA_ARGS__); break;
#define ENABLE_ALL_REG(ins, ...) \
    CASE(ins ## _B, __VA_ARGS__, BC._1) \
    CASE(ins ## _C, __VA_ARGS__, BC._0) \
    CASE(ins ## _D, __VA_ARGS__, DE._1) \
    CASE(ins ## _E, __VA_ARGS__, DE._0) \
    CASE(ins ## _H, __VA_ARGS__, HL._1) \
    CASE(ins ## _L, __VA_ARGS__, HL._0) \
    CASE(ins ## _M, __VA_ARGS__, M) \
    CASE(ins ## _A, __VA_ARGS__, PS.A) 
#define ENABLE_ALL_PAIR(ins, ...) \
    CASE(ins ## _B, __VA_ARGS__, BC) \
    CASE(ins ## _D, __VA_ARGS__, DE) \
    CASE(ins ## _H, __VA_ARGS__, HL) \
    CASE(ins ## _SP, __VA_ARGS__, SP)
#define ENABLE_ALL_FLAG(ins, fn) \
    CASE(ins ## Z, fn, PS.Z) \
    CASE(ins ## NZ, fn, !PS.Z) \
    CASE(ins ## P, fn, !PS.S) \
    CASE(ins ## M, fn, PS.S) \
    CASE(ins ## C, fn, PS.CY) \
    CASE(ins ## NC, fn, !PS.CY) \
    CASE(ins ## PE, fn, PS.P) \
    CASE(ins ## PO, fn, !PS.P)
#define ENABLE_ALL_DST_SRC(ins, fn) \
    ENABLE_ALL_REG(ins ## _B, fn, BC._1) \
    ENABLE_ALL_REG(ins ## _C, fn, BC._0) \
    ENABLE_ALL_REG(ins ## _D, fn, DE._1) \
    ENABLE_ALL_REG(ins ## _E, fn, DE._0) \
    ENABLE_ALL_REG(ins ## _H, fn, HL._1) \
    ENABLE_ALL_REG(ins ## _L, fn, HL._0) \
    ENABLE_ALL_REG(ins ## _A, fn, PS.A)

        switch (IR) {

            case NOP: break;

            // Stack, I/O and Machine Control

            CASE(HLT,   hlt)
            CASE(DI,    di)
            CASE(EI,    ei)
            CASE(RIM,   rim)
            CASE(SIM,   sim)

            CASE(OUT,   out)
            CASE(IN,    in)

            CASE(XTHL,  xthl)
            CASE(SPHL,  sphl)

            CASE(POP_B,     pop, BC)
            CASE(POP_D,     pop, DE)
            CASE(POP_H,     pop, HL)
            CASE(POP_PSW,   pop, PS)

            CASE(PUSH_B,    push, BC)
            CASE(PUSH_D,    push, DE)
            CASE(PUSH_H,    push, HL)
            CASE(PUSH_PSW,  push, PS)

            // Branching instructions

            CASE(PCHL, pchl)

            CASE(RST_0, rst, 0x0000)
            CASE(RST_1, rst, 0x0008)
            CASE(RST_2, rst, 0x0010)
            CASE(RST_3, rst, 0x0018)
            CASE(RST_4, rst, 0x0020)
            CASE(RST_5, rst, 0x0028)
            CASE(RST_6, rst, 0x0030)
            CASE(RST_7, rst, 0x0038)

            CASE(JMP, jmp, true)
            ENABLE_ALL_FLAG(J, jmp)

            CASE(CALL, call, true)
            ENABLE_ALL_FLAG(C, call)

            CASE(RET, ret, true)
            ENABLE_ALL_FLAG(R, ret)

            // Data transfer instructions

            CASE(XCHG,      xchg)

            CASE(LDAX_B,    ldax, BC)
            CASE(LDAX_D,    ldax, DE)
            CASE(LHLD,      lhld)
            CASE(LDA,       lda)

            CASE(STAX_B,    stax, BC)
            CASE(STAX_D,    stax, DE)
            CASE(SHLD,      shld)
            CASE(STA,       sta)

            ENABLE_ALL_PAIR(    LXI, lxi)

            ENABLE_ALL_REG(     MVI, mvi)

            ENABLE_ALL_DST_SRC( MOV, mov)

            CASE(MOV_M_A, mov, M, PS.A)
            CASE(MOV_M_B, mov, M, BC._1)
            CASE(MOV_M_C, mov, M, BC._0)
            CASE(MOV_M_D, mov, M, DE._1)
            CASE(MOV_M_E, mov, M, DE._0)
            CASE(MOV_M_H, mov, M, HL._1)
            CASE(MOV_M_L, mov, M, HL._0)

            // Arithmetic instructions

            CASE(DAA, daa)

            CASE(ADI, adi)
            CASE(ACI, aci)
            CASE(SUI, sui)
            CASE(SBI, sbi)

            ENABLE_ALL_PAIR(DAD, dad)

            ENABLE_ALL_PAIR(INX, inx)
            ENABLE_ALL_PAIR(DCX, dcx)

            ENABLE_ALL_REG(INR, inr)
            ENABLE_ALL_REG(DCR, dcr)

            ENABLE_ALL_REG(ADD, add)
            ENABLE_ALL_REG(ADC, adc)

            ENABLE_ALL_REG(SUB, sub)
            ENABLE_ALL_REG(SBB, sbb)

            // Logical instructions

            CASE(CMA, cma)

            CASE(STC, stc)
            CASE(CMC, cmc)

            CASE(RLC, rlc)
            CASE(RRC, rrc)
            CASE(RAL, ral)
            CASE(RAR, rar)

            CASE(ANI, ani)
            CASE(XRI, xri)
            CASE(ORI, ori)
            CASE(CPI, cpi)

            ENABLE_ALL_REG(ANA, ana)
            ENABLE_ALL_REG(XRA, xra)
            ENABLE_ALL_REG(ORA, ora)
            ENABLE_ALL_REG(CMP, cmp)

            default:
                serr << "Unsupported instruction";
        }
        // cycles += INS_CYCLES[ins]; // TODO INS_CYCLES
#undef ENABLE_ALL_DST_SRC
#undef ENABLE_ALL_FLAG
#undef ENABLE_ALL_PAIR
#undef ENABLE_ALL_REG
#undef CASE
#undef M
    }
}

// !SECTION Main functions

} // namespace intel_8085