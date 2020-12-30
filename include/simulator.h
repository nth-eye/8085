#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QStringListModel>

#include "memory.h"
#include "assembler.h"

enum Format     { HEX, DEC, BIN };
enum Order      { LSB, MSB, WORD };
enum RegName    { A, B, C, D, E, H, L, M, F, REG_NUM };

inline RegName& operator++(RegName &reg) { return reg = RegName(int(reg) + 1); }

class Simulator : public QObject {
    Q_OBJECT
    Q_PROPERTY(StackModel *stack_model READ get_stack)
    Q_PROPERTY(MemoryModel *memory_model READ get_memory)
    Q_PROPERTY(QStringListModel *reg_model READ get_registers NOTIFY registersChanged)
    Q_PROPERTY(QStringListModel *flags_model READ get_flags NOTIFY flagsChanged)
    Q_PROPERTY(int pc READ get_pc NOTIFY pcChanged)
    Q_PROPERTY(int sp READ get_sp NOTIFY spChanged)
    Q_PROPERTY(int format READ get_format WRITE set_format NOTIFY formatChanged)
public:
    Simulator(QObject *parent = 0);

    Q_INVOKABLE void assemble(const QString file);
private:
    StackModel*         get_stack()     { return &stack_model; }
    MemoryModel*        get_memory()    { return &memory_model; }
    QStringListModel*   get_registers() { return reg_model; }
    QStringListModel*   get_flags()     { return flags_model; }
    int                 get_pc()        { return PC; }
    int                 get_sp()        { return SP; }
    int                 get_format()    { return format; }

    void set_register(RegName reg, Byte byte);
    void set_flags(int new_f)   { regs[F] = new_f; update_flags(); }
    void set_pc(Word word)      { PC = word; emit pcChanged(); }
    void set_sp(Word word)      { SP = word; emit spChanged(); }
    void set_format(int new_f)  { format = new_f; update_registers(); }
    void set_byte(Word addr, Byte byte);
    void set_pzs(Byte);
    void set_carry(bool);
    void set_carry(Byte, Byte);
    void set_aux_carry(Byte, Byte);
    void set_aux_carry(bool);

    void update_registers();
    void update_flags();

    void exec(size_t);
    void reset();

    Byte fetch_byte();
    Word fetch_word();

    Word to_word(RegName, RegName);

    // Stack, I/O and Machine Control

    void pop(RegName, RegName);
    void push(Byte, Byte);

    // Branching instructions

    void rst(Word);
    void jmp(bool);
    void call(bool);
    void ret(bool);

    // Data transfer instructions

    void lxi(RegName, RegName);
    void mvi(RegName);
    void mov(RegName, RegName);

    // Arithmetic instructions

    void dad(RegName, RegName);
    void inx(RegName, RegName);
    void dcx(RegName, RegName);
    void inr(RegName);
    void dcr(RegName);
    void add(RegName);
    void adc(RegName);
    void sub(RegName);
    void sbb(RegName);

    // Logical instructions

    void cmc();
    void ana(RegName);
    void xra(RegName);
    void ora(RegName);
    void cmp(RegName);

private:
    int format = HEX;
    int base = 16;
    int padding = 2;

    std::pair<Word, Word> rom_range = { 0x0000, 0x2000 };
    std::unordered_map<ByteType, std::map<Word, Byte>> asm_codes;

    Memory      mem = {};
    Assembler   assembler{asm_codes};

    QStringListModel *reg_model;
    QStringListModel *flags_model;
    StackModel stack_model{mem};
    MemoryModel memory_model{mem};

    std::array<Reg, REG_NUM> regs = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
    Word PC;
    Word SP;

    bool halted = false;        // Is halted
    bool IE = false;            // Interrupts enabled

    bool M55, M65, M75;         // Masks for interrupts
    bool RST75_FF;              // RST 7.5 memory / flip-flop

//    Pin X1, X2;
    Pin SOD, SID;
    Pin TRAP, RST75, RST65, RST55, INTR, INTA; // 24, 3C, 34, 2C
//    Pin VSS, VCC;
//    Pin HOLD, HLDA;
//    Pin CLOCK;
//    Pin RESET_IN, RESET_OUT;
//    Pin READY;
//    Pin IO_M, S0, S1;
//    Pin ALE, RD, WR;

    size_t cycles   = 0;        // Clock cycles
signals:
    void registersChanged();
    void flagsChanged();
    void pcChanged();
    void spChanged();
    void formatChanged();
};

#endif // SIMULATOR_H
