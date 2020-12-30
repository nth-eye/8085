#include <regex>
#include <iomanip>
#include <QDebug>
		
#include "assembler.h"

namespace {

const std::string STR_BD = "B D";
const std::string STR_BDHSP = "B D H SP";
const std::string STR_BDHPSW = "B D H PSW";
const std::string STR_BCDEHLMA = "B C D E H L M A";

const std::unordered_map<std::string, Directive> DIR_MAP = {
    {"DB",      DB},
    {"DW",      DW},
    {"DS",      DS},
    {"ORG",     ORG}
};

const std::unordered_map<std::string, Instruction> INS_MAP = {
    {"ACI",     {ACI,       IMMEDIATE}},
    {"ADC",     {ADC_B,     BCDEHLMA}},
    {"ADD",     {ADD_B,     BCDEHLMA}},
    {"ADI",     {ADI,       IMMEDIATE}},
    {"ANA",     {ANA_B,     BCDEHLMA}},
    {"ANI",     {ANI,       IMMEDIATE}},
    {"CALL",    {CALL,      ADDRESS}},
    {"CMA",     {CMA}},
    {"CMC",     {CMC}},
    {"CMP",     {CMP_B,     BCDEHLMA}},
    {"CPI",     {CPI,       IMMEDIATE}},
    {"DAA",     {DAA}},
    {"DAD",     {DAD_B,     BDH_SP}},
    {"DCX",     {DCX_B,     BDH_SP}},
    {"DCR",     {DCR_B,     BCDEHLMA}},
    {"DI",      {DI}},
    {"EI",      {EI}},
    {"HLT",     {HLT}},
    {"IN",      {IN,        IMMEDIATE}},
    {"INR",     {INR_B,     BCDEHLMA}},
    {"INX",     {INX_B,     BDH_SP}},
    {"JC",      {JC,        ADDRESS}},
    {"JMP",     {JMP,       ADDRESS}},
    {"JNC",     {JNC,       ADDRESS}},
    {"JNZ",     {JNZ,       ADDRESS}},
    {"JP",      {JP,        ADDRESS}},
    {"JPE",     {JPE,       ADDRESS}},
    {"JPO",     {JPO,       ADDRESS}},
    {"JZ",      {JZ,        ADDRESS}},
    {"LDA",     {LDA,       ADDRESS}},
    {"LDAX",    {LDAX_B,    BD}},
    {"LHLD",    {LHLD,      ADDRESS}},
    {"LXI",     {LXI_B,     BDH_SP,     ADDRESS}},
    {"MOV",     {MOV_B_B,   BCDEHLMA,   BCDEHLMA}},
    {"MVI",     {MVI_B,     BCDEHLMA,   IMMEDIATE}},
    {"NOP",     {NOP}},
    {"ORA",     {ORA_B,     BCDEHLMA}},
    {"ORI",     {ORI,       IMMEDIATE}},
    {"OUT",     {OUT,       IMMEDIATE}},
    {"PCHL",    {PCHL}},
    {"POP",     {POP_B,     BDH_PSW}},
    {"PUSH",    {PUSH_B,    BDH_PSW}},
    {"RAL",     {RAL}},
    {"RAR",     {RAR}},
    {"RC",      {RC}},
    {"RET",     {RET}},
    {"RIM",     {RIM}},
    {"RLC",     {RLC}},
    {"RM",      {RM}},
    {"RNC",     {RNC}},
    {"RNZ",     {RNZ}},
    {"RP",      {RP}},
    {"RPE",     {RPE}},
    {"RPO",     {RPO}},
    {"RRC",     {RRC}},
    {"RST",     {RST_0,     RESET_NUMBER}},
    {"RZ",      {RZ}},
    {"SBB",     {SBB_B,     BCDEHLMA}},
    {"SBI",     {SBI,       IMMEDIATE}},
    {"SHLD",    {SHLD,      ADDRESS}},
    {"SIM",     {SIM}},
    {"SPHL",    {SPHL}},
    {"STA",     {STA,       ADDRESS}},
    {"STAX",    {STAX_B,    BD}},
    {"STC",     {STC}},
    {"SUB",     {SUB_B,     BCDEHLMA}},
    {"SUI",     {SUI,       IMMEDIATE}},
    {"XCHG",    {XCHG}},
    {"XRA",     {XRA_B,     BCDEHLMA}},
    {"XRI",     {XRI,       IMMEDIATE}},
    {"XTHL",    {XTHL}}
};

const std::unordered_map<Opcode, std::string> OPCODE_MAP = {
    {NOP,   "nop"},
    {}
};

} // namespace

void Assembler::preprocess(const std::string &src_file, const std::string &preproc_file)
{
    std::ifstream ifs;
    std::ofstream ofs;

    ifs.open(src_file);
    ofs.open(preproc_file);

    if (!ifs.is_open()) {
        sout << "Failed to open " << src_file;
        return;
    }

    std::string line;

    while (std::getline(ifs, line, '\r')) {

        ++line_num;
        
        if (line.empty())
            continue;

        std::istringstream ss(line);
        std::string word;

        if (line.find(':') == std::string::npos) //line.back() != ':')
            ofs << '\t';

        //line = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");

        while (ss >> word) {
            // Skip comments
            if (word[0] == ';')
                break;
            // To upper case
            std::transform(word.begin(), word.end(), word.begin(), ::toupper);
            
            // Output to preprocessed file
            ofs << word << ' ';
        }
        ofs << '\n';
    }
}

void Assembler::compile(const std::string &preproc_file, const std::string &exec_file)
{
    std::ifstream ifs;
    std::ofstream ofs;

    ifs.open(preproc_file);
    ofs.open(exec_file);

    if (!ifs.is_open()) {
        sout << "Failed to open " << preproc_file;
        return;
    }

    std::string line;

    line_num = 0;

    while (std::getline(ifs, line)) {

        ++line_num;

        if (line.back() == '\r')
            line.pop_back();

        std::istringstream ss(line);
        std::string word;

        while (ss >> word) {
            // Check if label. If not - must be instruction
            if (word.back() == ':') {
                word.pop_back();
                if (labels.contains(word)) 
                    sout << "Label " << word << " was already defined";
                else
                    labels.emplace(word, loc_cnt);
            } else if (auto it = DIR_MAP.find(word); it != DIR_MAP.end()) {
                parse_dir(ss, it->second);
                break;
            } else {
                parse_ins(ss, word);
                break;
            }
        }
    }

    // Replace all possible labels with confirmed
    for (const auto &[lbl, mem_ptr] : maybe_labels) {
        if (labels.find(lbl) != labels.end()) {
            asm_codes[OPERAND][mem_ptr] = labels[lbl] & 0xFF;
            asm_codes[OPERAND][mem_ptr + 1] = labels[lbl] >> 8;
        } else {
            sout << "Unrecognized label " << lbl;
        }
    }

    sout << "\nExec binary size: " << asm_codes.size();

//    for (auto [addr, byte] : asm_codes)
//        ofs << std::hex << std::setfill('0') << std::setw(4) << addr << '\t' << std::setw(2) << int(byte) << '\r';
}

void Assembler::shift_opcode(Opcode &code, int shift, char op)
{
    switch (op) {
        case 'B': break;
        case 'C': code += shift * 1; break;
        case 'D': code += shift * 2; break;
        case 'E': code += shift * 3; break;
        case 'H': code += shift * 4; break;
        case 'L': code += shift * 5; break;
        case 'M': code += shift * 6; break;
        case 'A': code += shift * 7; break;

        case 'S': code += shift * 6; break;
        case 'P': code += shift * 6; break;

        case '0': break;
        case '1': code += shift * 1; break;
        case '2': code += shift * 2; break;
        case '3': code += shift * 3; break;
        case '4': code += shift * 4; break;
        case '5': code += shift * 5; break;
        case '6': code += shift * 6; break;
        case '7': code += shift * 7; break;
    }
}

void Assembler::parse_dir(std::istringstream &iss, Directive dir)
{
    std::string op;

    switch (dir) {
        case DB:
            while (iss >> op) {
                if (op.back() == ',')
                    op.pop_back();
                if (legal_imm_data(op)) {
                    asm_codes[DATABYTE][loc_cnt] = operand_to_int(op);
                    ++loc_cnt;
                } else {
                    sout << "Illegal operand at line " << line_num;
                }
            }
            break;
        case DW:
            iss >> op;
            if (legal_address(op)) {
                Word data = operand_to_int(op);

                asm_codes[DATABYTE][loc_cnt++] = data & 0xFF;
                asm_codes[DATABYTE][loc_cnt++] = data >> 8;
            } else {
                sout << "Illegal operand at line " << line_num;
            }
            break;
        case DS:
            iss >> op;
            if (legal_address(op)) {
                loc_cnt += operand_to_int(op);
            } else {
                sout << "Illegal operand at line " << line_num;
            }
            break;
        case ORG:
            iss >> op;
            if (legal_address(op)) {
                loc_cnt = operand_to_int(op);
            } else {
                sout << "Illegal operand at line " << line_num;
            }
            break;
    }
}

void Assembler::parse_ins(std::istringstream &iss, std::string &ins_word)
{
    const auto it = INS_MAP.find(ins_word);
    if (it == INS_MAP.end()) {
        sout << "Unrecognized instruction " << ins_word << " on line " << line_num;
        return;
    }

    Instruction ins = it->second;

    std::string op_1, op_2, last;
    
    iss >> op_1 >> op_2 >> last;

    if (!last.empty()) {
        sout << "Illegal symbols after instruction " << ins_word << " on line " << line_num;
        return;
    }

    //sout << "Arguments on line " << line_num << ": " << op_1 << ' ' << op_2;

    if (ins.op_1_t != NO_OPERAND && op_1.empty()) {
        sout << "Operand must be provided with " << ins_word  << " on line " << line_num;
        return;
    }

    // If must be two operands - strip , from first
    if (op_1.back() == ',' && ins.op_2_t != NO_OPERAND)
        op_1.pop_back();

    if (!legal_operand(ins.op_1_t, op_1) || !legal_operand(ins.op_2_t, op_2)) {
        sout << "Invalid operands with " << ins_word  << " on line " << line_num;
        return;
    }

    switch (ins.code) {

        case STAX_B:
        case LDAX_B:
        case INX_B:
        case DCX_B:
        case MVI_B:
        case DAD_B:
        case LXI_B:
        case POP_B:
        case PUSH_B:
        case INR_B:
        case DCR_B: 
        case RST_0: shift_opcode(ins.code, 0x08, op_1[0]); break;

        case ADD_B:
        case ADC_B:
        case SUB_B:
        case SBB_B:
        case ANA_B:
        case XRA_B:
        case ORA_B:
        case CMP_B: shift_opcode(ins.code, 0x01, op_1[0]); break;

        case MOV_B_B: // Requires special handling
            if (op_1 == "M" && op_2 == "M") {
                sout << "MOV cannot have M both as src and dst on line" << line_num;
                return;
            }
            shift_opcode(ins.code, 0x08, op_1[0]);
            shift_opcode(ins.code, 0x01, op_2[0]);
            break;

        default: break;
    }
    asm_codes[INSTRUCTION][loc_cnt] = ins.code;

    ++loc_cnt;

    store_operand(ins.op_1_t, op_1);
    store_operand(ins.op_2_t, op_2);
}

void Assembler::store_operand(OperandType type, std::string &op)
{
    if (type == IMMEDIATE) {
        Byte data = operand_to_int(op);
        asm_codes[OPERAND][loc_cnt] = data;
        ++loc_cnt;
    } else if (type == ADDRESS) {

        Word data;

        auto it = labels.find(op);

        if (it != labels.end()) {
            data = it->second;
            if (maybe_labels.find(op) != maybe_labels.end()) {
                loc_cnt += 2; 
                return;
            }
        } else {
            data = operand_to_int(op);
        }

        asm_codes[OPERAND][loc_cnt] = data & 0xFF;
        ++loc_cnt;
        asm_codes[OPERAND][loc_cnt] = data >> 8;
        ++loc_cnt;
    }
}

bool Assembler::legal_operand(OperandType type, std::string &op)
{
    if (type == NO_OPERAND)
        return op.empty() ? true : false;

    if (op.empty())
        return false;
    
    if (type == IMMEDIATE) {

        return legal_imm_data(op) ? true : false;

    } else if (type == ADDRESS) {

        return legal_address(op) ? true : false;

    } else if (type == RESET_NUMBER) {

        if (op.size() == 1)
            return op[0] >= '0' && op[0] <= '8' ? true : false;
        return false;

    } else {
        // Relevant registers
        const std::string *regs;

        switch (type) {
            case BD:        regs = &STR_BD; break;
            case BDH_SP:    regs = &STR_BDHSP; break;
            case BDH_PSW:   regs = &STR_BDHPSW; break;
            case BCDEHLMA:  regs = &STR_BCDEHLMA; break;
            default: return false;
        }
        return regs->find(op) != std::string::npos ? true : false;
    }
    return false;
}

bool Assembler::legal_imm_data(std::string &op) // TODO ASCII strings and location counter
{ 
    // Check if single ASCII symbol
    if (op.size() == 3 && op[0] == '\'' && op.back() == '\'')
        return true;

    if (operand_to_int(op) <= 0xFF)
        return true;

    return false;
}

bool Assembler::legal_address(std::string &op)
{
    if (labels.find(op) != labels.end())
        return true;

    if (maybe_labels.find(op) != maybe_labels.end()) {
        return true; }

    if (operand_to_int(op) <= 0xFFFF) {
        return true;}

    if (legal_label(op)) {
        maybe_labels[op] = loc_cnt + 1;
        return true;
    }

    return false;
}

bool Assembler::legal_label(std::string &lbl)
{
    if (lbl.size() > 6)
        return false;

    if (lbl[0] < '?' || lbl[0] > 'Z')
        return false;

    return lbl.find_first_not_of("0123456789?@ABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos;

    // return std::regex_match(lbl, std::regex("[0-9?@A-Z]+"));
}

int Assembler::operand_to_int(std::string op)
{
    int base = 10;

    switch (op.back()) {
        case 'D': base = 10; op.pop_back(); break;
        case 'H': base = 16; op.pop_back(); break;
        case 'Q': base = 8; op.pop_back(); break;
        case 'B': base = 2; op.pop_back(); break;
    }

    int val;

    try {
        val = std::stoi(op, 0, base);
    } catch (...) {
        return 0x10000; // If conversion fails - return illegal value
    }
    return val;
}
