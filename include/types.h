#include <cstdint>
#include <cstddef>

#pragma once

namespace intel_8085 {

using Word = uint16_t;
using Byte = uint8_t;
using Pin = bool;

union RegPair {

    RegPair() = default;
    RegPair(Word word_) { word = word_; }

    explicit operator Word() { return word; }

    Word& operator++() { ++word; return word; }
    Word& operator--() { --word; return word; }
    Word operator++(int) { RegPair tmp(*this); operator++(); return tmp.word; }
    Word operator--(int) { RegPair tmp(*this); operator--(); return tmp.word; }

    RegPair& operator+=(const RegPair &reg) { word += reg.word; return *this; }
    RegPair& operator-=(const RegPair &reg) { word -= reg.word; return *this; }

    Word word;
    struct {
        Byte _0;
        Byte _1;
    };
    struct {
        Byte
        CY : 1, // Carry flag
        D1 : 1, // D1
        P  : 1, // Parity flag
        D3 : 1, // D3
        AC : 1, // Auxiliary Carry
        D5 : 1, // D5
        Z  : 1, // Zero flag
        S  : 1; // Sign flag
        Byte A; // A register alias, same as _1
    };
};

union Bus {
    Word word;
    struct {
        union {
            Byte
            D0 : 1, // Carry flag
            D1 : 1, // D1
            D2 : 1, // Parity flag
            D3 : 1, // D3
            D4 : 1, // Auxiliary Carry
            D5 : 1, // D5
            D6 : 1, // Zero flag
            D7 : 1; // Sign flag
            Byte data_bus;
        };
        union {
            Byte
            A8 : 1, // Carry flag
            A9 : 1, // D1
            A10 : 1, // Parity flag
            A11 : 1, // D3
            A12 : 1, // Auxiliary Carry
            A13 : 1, // D5
            A14 : 1, // Zero flag
            A15 : 1; // Sign flag
            Byte addr_bus;
        };
    };
};

// Machine cycle
enum Cycle : Byte {
    OF,     // Opcode fetch
    MR,     // Memory read
    MW,     // Memory write
    IOR,    // I/O read
    IOW,    // I/O write
    INA,    // Acknowledge of INTR
    BI      // Bus idle
};

// All machine codes
enum Opcode : Byte {

    // Stack, I/O and Machine Control

    NOP,
    HLT     = 0x76, // Halt and enter wait state
    DI      = 0xF3, // Disable interrupts
    EI      = 0xFB, // Enable interrupts
    RIM     = 0x20, // Read interrupt mask
    SIM     = 0x30, // Set interrupt mask

    OUT     = 0xD3,
    IN      = 0xDB,

    XTHL    = 0xE3,
    SPHL    = 0xF9,

    POP_B   = 0xC1,
    POP_D   = 0xD1,
    POP_H   = 0xE1,
    POP_PSW = 0xF1,

    PUSH_B  = 0xC5,
    PUSH_D  = 0xD5,
    PUSH_H  = 0xE5,
    PUSH_PSW= 0xF5,

    // Branching instructions

    PCHL    = 0xE9,

    RST_0   = 0xC7,
    RST_1   = 0xCF,
    RST_2   = 0xD7,
    RST_3   = 0xDF,
    RST_4   = 0xE7,
    RST_5   = 0xEF,
    RST_6   = 0xF7,
    RST_7   = 0xFF,

    JMP     = 0xC3,
    JZ      = 0xCA,
    JNZ     = 0xC2,
    JP      = 0xF2,
    JM      = 0xFA,
    JC      = 0xDA,
    JNC     = 0xD2,
    JPE     = 0xEA,
    JPO     = 0xE2,

    CALL    = 0xCD,
    CZ      = 0xCC,
    CNZ     = 0xC4,
    CP      = 0xF4,
    CM      = 0xFC,
    CC      = 0xDD,
    CNC     = 0xD4,
    CPE     = 0xEC,
    CPO     = 0xE4,

    RET     = 0xC9,
    RZ      = 0xC8,
    RNZ     = 0xC0,
    RP      = 0xF0,
    RM      = 0xF8,
    RC      = 0xD8,
    RNC     = 0xD0,
    RPE     = 0xE8,
    RPO     = 0xE0,

    // Data transfer instructions

    XCHG    = 0xEB,

    LDAX_B  = 0x0A,
    LDAX_D  = 0x1A,
    LHLD    = 0x2A,
    LDA     = 0x3A,

    STAX_B  = 0x02,
    STAX_D  = 0x12,
    SHLD    = 0x22,
    STA     = 0x32,

    LXI_B   = 0x01,
    LXI_D   = 0x11,
    LXI_H   = 0x21,
    LXI_SP  = 0x31,

    MVI_B   = 0x06,
    MVI_C   = 0x0E,
    MVI_D   = 0x16,
    MVI_E   = 0x1E,
    MVI_H   = 0x26,
    MVI_L   = 0x2E,
    MVI_M   = 0x36,
    MVI_A   = 0x3E,

    MOV_B_B = 0x40,
    MOV_B_C,
    MOV_B_D,
    MOV_B_E,
    MOV_B_H,
    MOV_B_L,
    MOV_B_M,
    MOV_B_A,

    MOV_C_B,
    MOV_C_C,
    MOV_C_D,
    MOV_C_E,
    MOV_C_H,
    MOV_C_L,
    MOV_C_M,
    MOV_C_A,

    MOV_D_B,
    MOV_D_C,
    MOV_D_D,
    MOV_D_E,
    MOV_D_H,
    MOV_D_L,
    MOV_D_M,
    MOV_D_A,

    MOV_E_B,
    MOV_E_C,
    MOV_E_D,
    MOV_E_E,
    MOV_E_H,
    MOV_E_L,
    MOV_E_M,
    MOV_E_A,

    MOV_H_B,
    MOV_H_C,
    MOV_H_D,
    MOV_H_E,
    MOV_H_H,
    MOV_H_L,
    MOV_H_M,
    MOV_H_A,

    MOV_L_B,
    MOV_L_C,
    MOV_L_D,
    MOV_L_E,
    MOV_L_H,
    MOV_L_L,
    MOV_L_M,
    MOV_L_A,

    MOV_M_B,
    MOV_M_C,
    MOV_M_D,
    MOV_M_E,
    MOV_M_H,
    MOV_M_L,
    
    MOV_M_A = 0x77,

    MOV_A_B,
    MOV_A_C,
    MOV_A_D,
    MOV_A_E,
    MOV_A_H,
    MOV_A_L,
    MOV_A_M,
    MOV_A_A,

    // Arithmetic instructions

    DAA     = 0x27,

    ADI     = 0xC6,
    ACI     = 0xCE,
    SUI     = 0xD6,
    SBI     = 0xDE,

    DAD_B   = 0x09,
    DAD_D   = 0x19,
    DAD_H   = 0x29,
    DAD_SP  = 0x39,

    INX_B   = 0x03,
    INX_D   = 0x13,
    INX_H   = 0x23,
    INX_SP  = 0x33,

    DCX_B   = 0x0B,
    DCX_D   = 0x1B,
    DCX_H   = 0x2B,
    DCX_SP  = 0x3B,

    INR_B   = 0x04,
    INR_C   = 0x0C,
    INR_D   = 0x14,
    INR_E   = 0x1C,
    INR_H   = 0x24,
    INR_L   = 0x2C,
    INR_M   = 0x34,
    INR_A   = 0x3C,

    DCR_B   = 0x05,
    DCR_C   = 0x0D,
    DCR_D   = 0x15,
    DCR_E   = 0x1D,
    DCR_H   = 0x25,
    DCR_L   = 0x2D,
    DCR_M   = 0x35,
    DCR_A   = 0x3D,

    ADD_B   = 0x80,
    ADD_C,
    ADD_D,
    ADD_E,
    ADD_H,
    ADD_L,
    ADD_M,
    ADD_A,

    ADC_B,
    ADC_C,
    ADC_D,
    ADC_E,
    ADC_H,
    ADC_L,
    ADC_M,
    ADC_A,

    SUB_B,
    SUB_C,
    SUB_D,
    SUB_E,
    SUB_H,
    SUB_L,
    SUB_M,
    SUB_A,

    SBB_B,
    SBB_C,
    SBB_D,
    SBB_E,
    SBB_H,
    SBB_L,
    SBB_M,
    SBB_A,

    // Logical instructions

    CMA     = 0x2F,

    STC     = 0x37,
    CMC     = 0x3F,

    RLC     = 0x07,
    RRC     = 0x0F,
    RAL     = 0x17,
    RAR     = 0x1F,

    ANI     = 0xE6,
    XRI     = 0xEE,
    ORI     = 0xF6,
    CPI     = 0xFE,
    
    ANA_B   = 0xA0,
    ANA_C,
    ANA_D,
    ANA_E,
    ANA_H,
    ANA_L,
    ANA_M,
    ANA_A,

    XRA_B,
    XRA_C,
    XRA_D,
    XRA_E,
    XRA_H,
    XRA_L,
    XRA_M,
    XRA_A,

    ORA_B,
    ORA_C,
    ORA_D,
    ORA_E,
    ORA_H,
    ORA_L,
    ORA_M,
    ORA_A,

    CMP_B,
    CMP_C,
    CMP_D,
    CMP_E,
    CMP_H,
    CMP_L,
    CMP_M,
    CMP_A,
};

constexpr Opcode  operator+(Opcode code, int v)     { return Opcode(int(code) + v); }
constexpr Opcode& operator+=(Opcode &code, int v)   { return code = code + v; }

} // namespace intel_8085