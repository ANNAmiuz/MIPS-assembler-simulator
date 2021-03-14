#ifndef CSC3050PRO1__ASSEMBLER_H
#define CSC3050PRO1__ASSEMBLER_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

enum Operation {
    Add = 1,
    Addu,
    Addi,
    Addiu,
    And,
    Andi,
    Clo,
    Clz,
    Div,
    Divu,
    Mult,
    Multu,
    Mul,
    Madd,
    Msub,
    Maddu,
    Msubu,
    Nor,
    Or,
    Ori,
    Sll,
    Sllv,//
    Sra,
    Srav,//
    Srl,
    Srlv,
    Sub,
    Subu,
    Xor,
    Xori,
    Lui,
    Slt,
    Sltu,
    Slti,
    Sltiu,
    Beq,
    Bgez,
    Bgezal,
    Bgtz,
    Blez,
    Bltzal,
    Bltz,
    Bne,
    J,
    Jal,
    Jalr,
    Jr,
    Teq,
    Teqi,
    Tne,
    Tnei,
    Tge,
    Tgeu,
    Tgei,
    Tgeiu,
    Tlt,
    Tltu,
    Tlti,
    Tltiu,
    Lb,
    Lbu,
    Lh,
    Lhu,
    Lw,
    Lwl,
    Lwr,
    Ll,
    Sb,
    Sh,
    Sw,
    Swl,
    Swr,
    Sc,
    Mfhi,
    Mflo,
    Mthi,
    Mtlo,
    Syscall
};

/*
*R-format: OP | rs | rt | rd | shamt | funct
*I-format: OP | rs | rt | immediate (16)
*J-format: OP | jump target(26)
 */
struct Instruction {
    int op = 0;
    uint32_t rd = 0;
    uint32_t rs = 0;
    uint32_t rt = 0;
    int shamt = 0;
    int funct;
    int16_t imme;
    std::string target;
    std::string machine_code;
};

//check whether a string contains only digits
bool is_alldigit(const std::string &str);

//tokenize the string by space
std::vector<std::string> tokenizer(std::string text);

// integer -> binary number
std::string ito_binary(int num, int digit);

//initialize mapping
void reg_mapping(std::map<std::string, uint32_t> &reg_map);

void opr_mapping(std::map<std::string, Operation> &opr);

//tokenize the input instruction
std::vector<std::string> tokenizer(std::string text);
std::vector<std::string> split(std::string& s, char delimiter);

//MIPS -> machine code
void translate(std::string instruction, int PC, std::map<std::string, Operation> opr_map,
               std::map<std::string, uint32_t> reg_map, std::vector<Instruction *> &MIPS_binary,
               std::map<std::string, unsigned int> symbol_table, int base_address);

//read twice the MIPS.txt file, delete comment, record label
void scan_process(std::map<std::string, Operation> opr_map, std::map<std::string, uint32_t> reg_map,
                  std::map<std::string, unsigned int> &symbol_table, std::vector<Instruction *> &MIPS_binary,
                  std::vector<std::string> &MIPS_data,
                  uint32_t base_address);

#endif //CSC3050PRO1__ASSEMBLER_H
