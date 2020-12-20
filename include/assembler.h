#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "types.h"
#include "misc.h"
#include "memory.h"

#pragma once

namespace intel_8085 {

// All directives (pseudo opcodes)
enum Directive {
    // EQU, SET,
    DB, DW,
    DS,
    // IF, ELSE, ENDIF, 
    // END, 
    // EOT,
    // ASEG, DSEG, CSEG, 
    ORG,
    // PUBLIC, EXTRN, NAME, STKLN,
    // MACRO, ENDM, LOCAL
    // LIB, LINK, LOCATE
    // STACK, MEMORY
    // REPT, IRP, IRPC, EXITM
};

enum OperandType {
    NO_OPERAND,
    BD,
    BDH_SP,
    BDH_PSW,
    BCDEHLMA,
    IMMEDIATE,
    ADDRESS,
    RESET_NUMBER
};

struct Assembler {

    Assembler(Memory &mem_) : mem(mem_) {}

    void assemble(const std::string &file);
    void preprocess();
    void compile();
    void link();

private:
    void shift_opcode(Opcode&, int, char);
    void parse_dir(std::istringstream&, Directive);
    void parse_ins(std::istringstream&, std::string&);
    void store_operand(OperandType, std::string&);
    bool legal_operand(OperandType, std::string&);
    bool legal_imm_data(std::string&);
    bool legal_address(std::string&);
    bool legal_label(std::string&);
    int operand_to_int(std::string);


    std::map<std::string, std::string>  sym_table;
    std::map<std::string, Word>         labels;
    std::map<std::string, Word>         maybe_labels;
    std::map<Word, Byte>                asm_codes;

    std::string filename;
    std::string preproc_filename;
    std::string exec_filename;

    Memory  &mem;

    size_t  line_num = 0;
    Word    loc_cnt = 0;
};

} // namespace intel_8085
