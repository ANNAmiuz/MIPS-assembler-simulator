#include "_assembler.h"
#include "simulator.h"
#include <iostream>


int main(int argc, char* argv[]) {
    //change for test
    std::string asm_path = "memcpy-hello-world.asm";
    std::string input_path = "memcpy-hello-world.in";
    std::string out_path = "memcpy-hello-world.out";
    //std::string asm_path = std::string(argv[1]);
    //std::string input_path = std::string(argv[2]);
    //std::string out_path = std::string(argv[3]);
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
    scan_process(opr_map, reg_map, symbol_table, MIPS_binary,MIPS_data,0x400000,asm_path);
    //test("fib_binary.txt", MIPS_binary);//for assembler
    //simulator
    unsigned char *real_memory;
    unsigned char *PC;
    unsigned char *heap_pointer;
    uint32_t *registers;
    init_space(registers, real_memory, MIPS_binary,MIPS_data, PC,heap_pointer);
    perform_code(registers, PC, real_memory,input_path,out_path,heap_pointer);
    return 0;
}
