#include "simulator.h"

#include <QDebug>
#include <iomanip>

Simulator::Simulator(QObject *parent) : QObject(parent)
{
    reg_model = new QStringListModel();
    flags_model = new QStringListModel();

    QStringList reg_list = { "A", "B", "C", "D", "E", "H", "L", "M" };
    QStringList flags_list = { "CY", "-", "P", "-", "AC", "-", "Z", "S" };

    reg_model->setStringList(reg_list);
    flags_model->setStringList(flags_list);

    update_registers();
    update_flags();
}

void Simulator::assemble(const QString file_)
{
    std::string file = file_.toStdString();
    std::string preproc_file = file;

    size_t lastindex = file.find_last_of(".");
    if (lastindex != std::string::npos)
        preproc_file = preproc_file.substr(0, lastindex);

    std::string exec_file = preproc_file;
    preproc_file += "_preproc.asm";
    exec_file += "_exec.txt";

    assembler.preprocess(file, preproc_file);
    assembler.compile(preproc_file, exec_file);

    // sout << "ROM addresses: " << std::hex << std::setw(4) << rom_range.first << " - " << rom_range.second;

    for (const auto &[type, byte_map] : asm_codes) {
        for (auto [addr, byte] : byte_map) {
            // sout << std::hex << std::setw(4) << addr << '\t' << int(byte) << '\r';
            set_byte(addr, byte);
        }
    }
    rom_range.second = asm_codes[INSTRUCTION].rbegin()->first;

    sout << "ROM addresses: " << std::hex << std::setw(4) << rom_range.first << " - " << rom_range.second;
}

void Simulator::set_register(RegName reg, Byte byte)
{
    regs[reg] = byte;

    if (reg == H || reg == L) {
        regs[M] = mem[to_word(H, L)];
        reg_model->setData(
            reg_model->index(M),
            QStringLiteral("%1").arg(regs[M], padding, base, QLatin1Char('0')).toUpper()
        );
    }

    if (reg == M)
        set_byte(to_word(H, L), byte);

    if (reg == F)
        update_flags();

    reg_model->setData(
        reg_model->index(reg),
        QStringLiteral("%1").arg(byte, padding, base, QLatin1Char('0')).toUpper()
    );
}

void Simulator::set_byte(Word addr, Byte byte)
{
    mem[addr] = byte;

    const auto list_idx = stack_model.index(addr);
    const auto table_idx = memory_model.index(addr >> 4, addr & 0xFF);

    emit stack_model.dataChanged(list_idx, list_idx, QVector<int>() << StackModel::ByteRole);
    emit memory_model.dataChanged(table_idx, table_idx, QVector<int>() << MemoryModel::ByteRole << MemoryModel::CharRole);
}

void Simulator::set_pzs(Byte res)
{
    // CY = not affected
    regs[F].P = std::popcount(res) & 1 ? 0 : 1;
     // AC = not affected
    regs[F].Z = res == 0;
    regs[F].S = res & 0b10000000 ? 1 : 0;
    for (const auto i : { 2, 6, 7 })
        flags_model->setData(flags_model->index(i), QString::number((regs[F] >> i) & 1));
}

void Simulator::set_carry(Byte op_1, Byte op_2)
{
    Word res = Word(op_1) + op_2;
    set_carry(res & 0xFF00);
}

void Simulator::set_carry(bool flag)
{
    regs[F].CY = flag;
    flags_model->setData(flags_model->index(0), QString::number(flag));
}

void Simulator::set_aux_carry(Byte op_1, Byte op_2)
{
    set_aux_carry(((op_1 & 0xF) + (op_2 & 0xF)) & 0xF0);
}

void Simulator::set_aux_carry(bool flag)
{
    regs[F].AC = flag;
    flags_model->setData(flags_model->index(4), QString::number(flag));
}

void Simulator::update_registers()
{
    switch (format) {
        case HEX: base = 16; padding = 2; break;
        case DEC: base = 10; padding = 3; break;
        case BIN: base = 2; padding = 8; break;
    }

    for (RegName reg = A; reg < F; ++reg)
        set_register(reg, regs[reg]);
}

void Simulator::update_flags()
{
    for (int i = 0; i < 8; ++i)
        flags_model->setData(
            flags_model->index(i),
            QString::number((regs[F] >> i) & 1));
}

void Simulator::exec(size_t num_ins)
{
    for (size_t i = 0; i < num_ins; ++i) {

        if (halted) {
            sout << "Processor is halted";
            return;
        }

        auto IR = fetch_byte();

        Byte tmp = 0;       // Temporary register, used not as in real architecture, but for storing ALU result to set flags
        Byte act = regs[A]; // Accumulator temporary register
        Word addr = 0;      // Temporary for word

#define CASE(ins, fn, ...) case ins: fn(__VA_ARGS__); break;
#define ENABLE_ALL_REG(ins, ...) \
    CASE(ins ## _B, __VA_ARGS__, B) \
    CASE(ins ## _C, __VA_ARGS__, C) \
    CASE(ins ## _D, __VA_ARGS__, D) \
    CASE(ins ## _E, __VA_ARGS__, E) \
    CASE(ins ## _H, __VA_ARGS__, H) \
    CASE(ins ## _L, __VA_ARGS__, L) \
    CASE(ins ## _M, __VA_ARGS__, M) \
    CASE(ins ## _A, __VA_ARGS__, A)
#define ENABLE_ALL_FLAGS(ins, fn) \
    CASE(ins ## Z, fn, regs[F].Z) \
    CASE(ins ## NZ, fn, !regs[F].Z) \
    CASE(ins ## P, fn, !regs[F].S) \
    CASE(ins ## M, fn, regs[F].S) \
    CASE(ins ## C, fn, regs[F].CY) \
    CASE(ins ## NC, fn, !regs[F].CY) \
    CASE(ins ## PE, fn, regs[F].P) \
    CASE(ins ## PO, fn, !regs[F].P)
#define ENABLE_ALL_MOV(ins, fn) \
    ENABLE_ALL_REG(ins ## _B, fn, B) \
    ENABLE_ALL_REG(ins ## _C, fn, C) \
    ENABLE_ALL_REG(ins ## _D, fn, D) \
    ENABLE_ALL_REG(ins ## _E, fn, E) \
    ENABLE_ALL_REG(ins ## _H, fn, H) \
    ENABLE_ALL_REG(ins ## _L, fn, L) \
    ENABLE_ALL_REG(ins ## _A, fn, A)

        switch (IR) {

            case NOP: break;

            // Stack, I/O and Machine Control

            case HLT: halted = true; break;
            case DI: IE = false; break;
            case EI: IE = true; break;
            case RIM:
                tmp |= M55;
                tmp |= M65 << 1;
                tmp |= M75 << 2;
                tmp |= IE << 3;
                tmp |= RST55 << 4;
                tmp |= RST65 << 5;
                tmp |= RST75_FF << 6;
                tmp |= SID << 7;
                set_register(A, tmp);
                break;
            case SIM:
                // SDE - Serial data enable - set to 1 for sending
                if (act & 0b01000000)
                    SOD = act & 0b10000000;
                // MSE - Mask set enable - set to 1 to mask interrupts
                if (act & 0b00000100) {
                    M75 = act & 0b00000100;
                    M65 = act & 0b00000010;
                    M55 = act & 0b00000001;
                }
                // Reset RTS 7.5 flip-flop
                if (tmp & 0b00010000)
                    RST75_FF = false;
                break;
            case OUT: break; // TODO OUT instruction
            case IN: break; // TODO IN instruction
            case XTHL:
                tmp = mem[SP];
                set_byte(SP, regs[L]);
                set_register(L, tmp);
                tmp = mem[SP + 1];
                set_byte(SP + 1, regs[H]);
                set_register(H, tmp);
                break;
            case SPHL: set_sp(to_word(H, L)); break;
            case POP_B: pop(B, C); break;
            case POP_D: pop(D, E); break;
            case POP_H: pop(H, L); break;
            case POP_PSW: pop(A, F); break;

            // Branching instructions

            case PCHL: set_pc(to_word(H, L)); break;
            case RST_0: rst(0x0000); break;
            case RST_1: rst(0x0008); break;
            case RST_2: rst(0x0010); break;
            case RST_3: rst(0x0018); break;
            case RST_4: rst(0x0020); break;
            case RST_5: rst(0x0028); break;
            case RST_6: rst(0x0030); break;
            case RST_7: rst(0x0038); break;
            case JMP: jmp(true); break;
            ENABLE_ALL_FLAGS(J, jmp)
            case CALL: call(true); break;
            ENABLE_ALL_FLAGS(C, call)
            case RET: ret(true); break;
            ENABLE_ALL_FLAGS(R, ret)

            // Data transfer instructions

            case XCHG:
                tmp = H;
                set_register(H, regs[D]);
                set_register(D, tmp);
                tmp = L;
                set_register(L, regs[E]);
                set_register(E, tmp);
                break;
            case LDAX_B: set_register(A, mem[to_word(B, C)]); break;
            case LDAX_D: set_register(A, mem[to_word(D, E)]); break;
            case LHLD:
                addr = fetch_word();
                set_register(L, mem[addr++]);
                set_register(H, mem[addr]);
                break;
            case LDA:
                addr = fetch_word();
                set_register(A, mem[addr]);
                break;
            case STAX_B: set_byte(to_word(B, C), act); break;
            case STAX_D: set_byte(to_word(D, E), act); break;
            case SHLD:
                addr = fetch_word();
                set_byte(addr++, regs[L]);
                set_byte(addr, regs[H]);
                break;
            case STA:
                addr = fetch_word();
                set_byte(addr, act);
                break;
            case LXI_B: lxi(B, C); break;
            case LXI_D: lxi(D, E); break;
            case LXI_H: lxi(H, L); break;
            case LXI_SP: set_sp(fetch_word()); break;
            ENABLE_ALL_REG(MVI, mvi)
            ENABLE_ALL_MOV(MOV, mov)
            CASE(MOV_M_A, mov, M, A)
            CASE(MOV_M_B, mov, M, B)
            CASE(MOV_M_C, mov, M, C)
            CASE(MOV_M_D, mov, M, D)
            CASE(MOV_M_E, mov, M, E)
            CASE(MOV_M_H, mov, M, H)
            CASE(MOV_M_L, mov, M, L)

            // Arithmetic instructions

            case DAA:
                if ((act & 0x0F) > 0x09 || regs[F].AC)
                    act += (tmp |= 0x06);
                if ((act & 0xF0) > 0x90 || regs[F].CY) {
                    act += 0x60;
                    tmp |= 0x60;
                }
                set_aux_carry(regs[A], tmp);
                set_carry(regs[A], tmp);
                set_register(A, act);
                set_pzs(regs[A]);
                break;
            case ADI:
                tmp = fetch_byte();
                set_carry(act, tmp);
                set_aux_carry(act, tmp);
                set_register(A, act + tmp);
                set_pzs(regs[A]);
                break;
            case ACI:
                tmp = fetch_byte() + regs[F].CY;
                set_carry(act, tmp);
                set_aux_carry(act, tmp);
                set_register(A, act + tmp);
                set_pzs(regs[A]);
                break;
            case SUI:
                tmp = fetch_byte();
                set_carry(act, -tmp);
                set_aux_carry(act, -tmp);
                set_register(A, act - tmp);
                set_pzs(regs[A]);
                cmc();
                break;
            case SBI:
                tmp = fetch_byte() + regs[F].CY;
                set_carry(act, -tmp);
                set_aux_carry(act, -tmp);
                set_register(A, act - tmp);
                set_pzs(regs[A]);
                cmc();
                break;
            case DAD_B: dad(B, C); break;
            case DAD_D: dad(D, E); break;
            case DAD_H: dad(M, L); break;
            // case DAD_SP: dad(SP); break; // TODO
            case INX_B: inx(B, C); break;
            case INX_D: inx(D, E); break;
            case INX_H: inx(M, L); break;
            // case INX_SP: inx(SP); break; // TODO
            case DCX_B: dcx(B, C); break;
            case DCX_D: dcx(D, E); break;
            case DCX_H: dcx(M, L); break;
            // case DCX_SP: dcx(SP); break; // TODO
            ENABLE_ALL_REG(INR, inr)
            ENABLE_ALL_REG(DCR, dcr)
            ENABLE_ALL_REG(ADD, add)
            ENABLE_ALL_REG(ADC, adc)
            ENABLE_ALL_REG(SUB, sub)
            ENABLE_ALL_REG(SBB, sbb)

            // Logical instructions

            case CMA: set_register(A, ~regs[A]); break;
            case STC: set_carry(true); break;
            case CMC: cmc(); break;
            case RLC:
                set_carry(act & 0b10000000);
                act <<= 1;
                act |= regs[F].CY;
                set_register(A, act);
                break;
            case RRC:
                set_carry(act & 0b00000001);
                act >>= 1;
                act |= regs[F].CY << 7;
                set_register(A, act);
                break;
            case RAL:
                tmp = act & 0b10000000;
                act <<= 1;
                act |= regs[F].CY;
                set_register(A, act);
                set_carry(tmp);
                break;
            case RAR:
                tmp = act & 0b00000001;
                act >>= 1;
                act |= regs[F].CY << 7;
                set_register(A, act);
                set_carry(tmp);
                break;
            case ANI:
                tmp = fetch_byte();
                set_register(A, regs[A] & tmp);
                set_carry(false);
                set_aux_carry(true);
                set_pzs(regs[A]);
                break;
            case XRI:
                tmp = fetch_byte();
                set_register(A, regs[A] ^ tmp);
                set_carry(false);
                set_aux_carry(false);
                set_pzs(regs[A]);
                break;
            case ORI:
                tmp = fetch_byte();
                set_register(A, regs[A] | tmp);
                set_carry(false);
                set_aux_carry(false);
                set_pzs(regs[A]);
                break;
            case CPI:
                tmp = fetch_byte();
                set_carry(act < tmp);
                set_aux_carry(act, -tmp);
                set_pzs(act - tmp);
                break;
            ENABLE_ALL_REG(ANA, ana)
            ENABLE_ALL_REG(XRA, xra)
            ENABLE_ALL_REG(ORA, ora)
            ENABLE_ALL_REG(CMP, cmp)

            default:
                serr << "Unsupported instruction: 0x" << std::hex << IR;
        }
        // cycles += INS_CYCLES[ins]; // TODO INS_CYCLES
#undef ENABLE_ALL_MOV
#undef ENABLE_ALL_FLAGS
#undef ENABLE_ALL_REG
#undef CASE
    }
}

void Simulator::reset()
{
    set_pc(0x1000);
    set_sp(mem.size() - 1);

    for (auto &it : regs)
        it = 0;

    cycles = 0;

    std::fill(std::begin(mem), std::begin(mem) + 0x10000, Byte(0));

    update_registers();
    update_flags();
}

Byte Simulator::fetch_byte()
{
    Word addr = PC;
    set_pc(PC + 1);
    return mem[addr];
}

Word Simulator::fetch_word()
{
    Word lsb = fetch_byte();
    Word msb = fetch_byte();
    return lsb | (msb << 8);
}

Word Simulator::to_word(RegName msb, RegName lsb)
{
    return Word(regs[msb] << 8) | regs[lsb];
}

// SECTION Instructions

void Simulator::pop(RegName msb, RegName lsb)
{
    set_register(lsb, mem[SP]);
    set_sp(SP + 1);
    set_register(msb, mem[SP]);
    set_sp(SP + 1);
}

void Simulator::push(Byte msb, Byte lsb)
{
    set_sp(SP - 1);
    set_byte(SP, msb);
    set_sp(SP - 1);
    set_byte(SP, lsb);
}

void Simulator::rst(Word addr)
{
    push(PC >> 8, PC & 0xFF);
    set_pc(addr);
}

void Simulator::jmp(bool flag)
{
    Word tmp = fetch_word();
    if (flag)
        set_pc(tmp);
}

void Simulator::call(bool flag)
{
    Word tmp = fetch_word();
    if (flag)
        rst(tmp);
}

void Simulator::ret(bool flag) // TODO
{
    if (flag) {
        Byte lsb = mem[SP];
        set_sp(SP + 1);
        Byte msb = mem[SP];
        set_sp(SP + 1);
        set_pc((Word(msb) << 8) | lsb);
    }
}

void Simulator::lxi(RegName msb, RegName lsb)
{
    set_register(lsb, fetch_byte());
    set_register(msb, fetch_byte());
}

void Simulator::mvi(RegName reg)
{
    Byte tmp = fetch_byte();
    set_register(reg, tmp);
}

void Simulator::mov(RegName dst, RegName src)
{
    set_register(dst, regs[src]);
}

void Simulator::dad(RegName msb, RegName lsb)
{
    Word tmp = to_word(H, L);
    Word res = tmp + to_word(msb, lsb);
    set_register(H, res >> 8);
    set_register(L, res & 0xFF);
    regs[F].CY = to_word(H, L) < tmp ? 1 : 0;
    flags_model->setData(flags_model->index(0), QString::number(regs[F] & 1));
}

void Simulator::inx(RegName msb, RegName lsb)
{
    Word word = to_word(msb, lsb);
    ++word;
    set_register(msb, word >> 8);
    set_register(lsb, word & 0xFF);
}

void Simulator::dcx(RegName msb, RegName lsb)
{
    Word word = to_word(msb, lsb);
    --word;
    set_register(msb, word >> 8);
    set_register(lsb, word & 0xFF);
}

void Simulator::inr(RegName reg)
{
    set_aux_carry(regs[reg], 1);
    set_register(reg, regs[reg] + 1);
    set_pzs(reg);
}

void Simulator::dcr(RegName reg)
{
    set_aux_carry(regs[reg], -1);
    set_register(reg, regs[reg] - 1);
    set_pzs(reg);
}

void Simulator::add(RegName reg)
{
    Byte tmp = regs[reg];
    set_carry(regs[A], tmp);
    set_aux_carry(regs[A], tmp);
    set_register(A, regs[A] + tmp);
    set_pzs(regs[A]);
}

void Simulator::adc(RegName reg)
{
    Byte tmp = regs[reg] + regs[F].CY;
    set_carry(regs[A], tmp);
    set_aux_carry(regs[A], tmp);
    set_register(A, regs[A] + tmp);
    set_pzs(regs[A]);
}

void Simulator::sub(RegName reg)
{
    Byte tmp = regs[reg];
    set_carry(regs[A], -tmp);
    set_aux_carry(regs[A], -tmp);
    set_register(A, regs[A] - tmp);
    set_pzs(regs[A]);
    cmc();
}

void Simulator::sbb(RegName reg)
{
    Byte tmp = regs[reg] + regs[F].CY;
    set_carry(regs[A], -tmp);
    set_aux_carry(regs[A], -tmp);
    set_register(A, regs[A] - tmp);
    set_pzs(regs[A]);
    cmc();
}

void Simulator::cmc()
{
    regs[F].CY = ~regs[F].CY;
    flags_model->setData(flags_model->index(0), QString::number(regs[F].CY));
}

void Simulator::ana(RegName reg)
{
    set_register(A, regs[A] & regs[reg]);
    set_carry(false);
    set_aux_carry(true);
    set_pzs(regs[A]);
}

void Simulator::xra(RegName reg)
{
    set_register(A, regs[A] ^ regs[reg]);
    set_carry(false);
    set_aux_carry(false);
    set_pzs(regs[A]);
}

void Simulator::ora(RegName reg)
{
    set_register(A, regs[A] | regs[reg]);
    set_carry(false);
    set_aux_carry(false);
    set_pzs(regs[A]);
}

void Simulator::cmp(RegName reg)
{
    set_carry(regs[A] < regs[reg]);
    set_aux_carry(regs[A], -regs[reg]);
    set_pzs(regs[A] - regs[reg]);
}

// !SECTION Instructions
