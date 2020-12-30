#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "types.h"

#pragma once

struct Assembler {

    Assembler(std::unordered_map<ByteType, std::map<Word, Byte>> &asm_codes_) : asm_codes(asm_codes_) {}

    void preprocess(const std::string &, const std::string &);
    void compile(const std::string &, const std::string &);
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

//    std::map<std::string, std::string>  sym_table;
    std::map<std::string, Word>         labels;
    std::map<std::string, Word>         maybe_labels;

    std::unordered_map<ByteType, std::map<Word, Byte>> &asm_codes;

    size_t  line_num = 0;
    Word    loc_cnt = 0;
};
