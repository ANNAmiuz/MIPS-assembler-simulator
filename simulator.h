#ifndef CSC3050PRO1_SIMULATOR_H
#define CSC3050PRO1_SIMULATOR_H

#include <vector>
#include <iostream>
#include <fstream>
#include<stdio.h>
#include<fcntl.h>
#include <unistd.h>
#include <regex>
#include "_assembler.h"

/*
* Registers: 0~31 normal MIPS regs
* Registers: 32: lo
* Registers: 33: hi
*/

/*
* ----------------- 0xA000000  real_memory[0x600000-1] end
* | Stack Segment  |
* -----------------
* | Dynamic data   |
* ----------- Data segment
* | Static data    |
* ----------------- 0x500000   real_memory[0x100000] start
* | Text segment   |
* ----------------- 0x400000   real_memory[0] start
*/
void init_space(uint32_t *&registers, unsigned char *&real_memory, std::vector<Instruction *> &MIPS_binary,
                std::vector<std::string> &MIPS_data,
                unsigned char *&PC, unsigned char *&heap);

struct _instruction {
    uint8_t op;
    uint8_t funct;
    uint8_t shamt;
    uint32_t rs;
    uint32_t rd;
    uint32_t rt;
    int16_t imme;
    int type = 0;
    uint32_t target;
};

//distinguish codes from binary to _instruction type
void pre_distinguish(unsigned char *PC, _instruction &cur, uint32_t ins);

//actual perform of MIPS codes
void perform_code(uint32_t *&registers, unsigned char *&PC, unsigned char *&real_mem, std::string
input_path, std::string output_path, unsigned char *&heap);

//deal with \" \' \t....in the input string
std::string string_clear(std::string str);

#endif //CSC3050PRO1_SIMULATOR_H