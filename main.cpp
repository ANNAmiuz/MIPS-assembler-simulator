#include "_assembler.h"
#include "simulator.h"
#include <iostream>

using namespace std;

int main() {
    //mapping reg string with number
    std::map<std::string, uint32_t> reg_map;
    //mapping operation string with number
    std::map<std::string, Operation> opr_map;
    //initialize mapping
    reg_mapping(reg_map);
    opr_mapping(opr_map);
    std::vector<std::string> MIPS_data;
    std::vector<Instruction *> MIPS_binary;
    //store the label address
    std::map<std::string, unsigned int> symbol_table;
    //assembler
    scan_process(opr_map, reg_map, symbol_table, MIPS_binary,MIPS_data,0x400000);

    //simulator
    unsigned char *real_memory;
    unsigned char *PC;
    uint32_t *registers;
    //init_space(registers, real_memory, MIPS_binary,MIPS_data, PC);
    //perform_code(registers, PC, real_memory);
    return 0;
}
