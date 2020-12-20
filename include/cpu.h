#include <utility>
#include <map>
#include <functional>
#include <bit>

#include "misc.h"
#include "memory.h"

#pragma once

namespace intel_8085 {

struct CPU {

    CPU(Memory mem_) : mem(mem_) {}

    void reset();
    void reset(Memory&);
    void exec(size_t);

    Word fetch_word();
    Byte fetch_byte();
    //Byte read_port(Byte);

private:
    void set_pzs(Byte);
    void set_carry(Byte, Byte);
    void set_aux_carry(Byte, Byte);

    // Stack, I/O and Machine Control

    void hlt();
    void di();
    void ei();
    void rim();
    void sim();

    void out();
    void in();

    void xthl();
    void sphl();
    
    void pop(RegPair&);
    void push(RegPair&);

    // Branching instructions

    void pchl();
    void rst(Word);
    void jmp(bool);
    void call(bool);
    void ret(bool);

    // Data transfer instructions

    void xchg();

    void ldax(RegPair&);
    void lhld();
    void lda();

    void stax(RegPair&);
    void shld();
    void sta();

    void lxi(RegPair&);
    void mvi(Byte&);
    void mov(Byte&, Byte&);

    // Arithmetic instructions

    void daa();

    void adi();
    void aci();
    void sui();
    void sbi();

    void dad(RegPair&);

    void inx(RegPair&);
    void dcx(RegPair&);
    void inr(Byte&);
    void dcr(Byte&);

    void add(Byte&);
    void adc(Byte&);
    void sub(Byte&);
    void sbb(Byte&);

    // Logical instructions

    void cma();

    void stc();
    void cmc();

    void rlc();
    void rrc();
    void ral();
    void rar();

    void ani();
    void xri();
    void ori();
    void cpi();

    void ana(Byte&);
    void xra(Byte&);
    void ora(Byte&);
    void cmp(Byte&);
    
private:
    Word PC     = 0x0000;       // Program counter

    RegPair SP  = 0x0000;       // Stack pointer
    RegPair PS  = 0;            // _0 is F & _1 is A. [ Program Status Word ] / [ Accumulator + Flags register ]

    RegPair BC  = 0;            // _0 is C
    RegPair DE  = 0;            // _0 is E
    RegPair HL  = 0;            // _0 is L

    // Word &M     = HL.word;      // Memory address register imitation

    // Byte TMP    = 0;            // Temporary register, used not as in real architecture, but for storing ALU result to set flags
    // Byte ACT    = 0;            // Accumulator temporary register
    Byte IR     = 0;            // Instruction register

    Memory &mem;

    size_t cycles   = 0;        // Clock cycles

    bool IE = false;            // Interrupts enabled
    bool halted = false;        // Is halted

    bool M55, M65, M75;         // Masks for interrupts 
    bool RST75_FF;              // RST 7.5 memory / flip-flop

    Pin X1, X2;
    Pin SOD, SID;
    Pin TRAP, RST75, RST65, RST55, INTR, INTA; // 24, 3C, 34, 2C
    Pin VSS, VCC;
    Pin HOLD, HLDA;
    Pin CLOCK;
    Pin RESET_IN, RESET_OUT;
    Pin READY;
    Pin IO_M, S0, S1;
    Pin ALE, RD, WR;

    Bus bus;
};

} // namespace intel_8085