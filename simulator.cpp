#include "simulator.h"
#include "MIPS_opr.h"

void init_space(uint32_t *&registers, unsigned char *&real_memory, std::vector<Instruction *> &MIPS_binary,
                std::vector<std::string> &MIPS_data,
                unsigned char *&PC) {
    /*
     * Registers: 0~31 normal MIPS regs
     * Registers: 32: lo
     * Registers: 33: hi
     */
    registers = new uint32_t[34];
    registers[0] = 0;
    /*
     * ----------------- 0xA00000   real_memory[0x600000-1] end
     * | Stack Segment  |
     * -----------------
     * | Dynamic data   |
     * ----------- Data segment
     * | Static data    |
     * ----------------- 0x500000   real_memory[0x100000] start
     * | Text segment
     * ----------------- 0x400000   real_memory[0] start
     */
    real_memory = new unsigned char[0x600000];
    PC = real_memory;
    registers[28] = 0x500000;
    registers[29] = 0xA00000;
    // store text segment
    for (int i = 0; i < MIPS_binary.size(); i++) {
        Instruction *current = MIPS_binary[i];
        real_memory[i * 4 + 3] = ((uint32_t) std::stoll(current->machine_code, nullptr, 2) & 0xff000000) >> 24;
        real_memory[i * 4 + 2] = ((uint32_t) std::stoll(current->machine_code, nullptr, 2) & 0x00ff0000) >> 16;
        real_memory[i * 4 + 1] = ((uint32_t) std::stoll(current->machine_code, nullptr, 2) & 0x0000ff00) >> 8;
        real_memory[i * 4 + 0] = (uint32_t) std::stoll(current->machine_code, nullptr, 2) & 0x000000ff;
    }
    //store data segment
    for (int i = 0; i < MIPS_data.size(); i++) {
        std::string cur = MIPS_data[i];
        int idx;
        if (cur.find(".asciiz") != std::string::npos) {
            cur = std::regex_replace(cur, std::regex(".asciiz"), "");
            std::string target = string_clear(cur);
            //std::cout<<target<<std::endl;
            for (int i = 0; i < target.length(); i++) {
                *get_real_address_from(registers[28], real_memory) = target[i];
                //std::cout<<(unsigned char)*get_real_address_from(registers[28], real_memory);
                registers[28] += 1;
            }
            *get_real_address_from(registers[28], real_memory) = 0x0;
            registers[28] += 1;
            if (registers[28] != 0)
                registers[28] += (4 - (registers[28] % 4));
        }
        else if (cur.find(".ascii")!= std::string::npos) {
            cur = std::regex_replace(cur, std::regex(".ascii"), "");
            std::string target = string_clear(cur);
            for (int i = 0; i < target.length(); i++) {
                *get_real_address_from(registers[28], real_memory) = target[i];
                registers[28] += 1;
            }
            if (registers[28] != 0)
                registers[28] += (4 - (registers[28] % 4));
        }  else if (cur.find(".word")!= std::string::npos) {
            idx = cur.find(".word");
            cur = cur.substr(idx + 5);
            cur.erase(0, cur.find_first_not_of(' '));
            std::vector<std::string> words = tokenizer(cur);
            for (int i = 0; i < words.size(); i++) {
                //simulator test
                int32_t w = std::stoi(words[i]);
                *get_real_address_from(registers[28], real_memory) = w & 0xff;
                *get_real_address_from(registers[28] + 1, real_memory) = (w & 0xff00) >> 8;
                *get_real_address_from(registers[28] + 2, real_memory) = (w & 0xff0000) >> 16;
                *get_real_address_from(registers[28] + 3, real_memory) = (w & 0xff000000) >> 24;
                registers[28] += 4;
            }
        } else if (cur.find(".byte")!= std::string::npos) {
            idx = cur.find(".byte");
            cur = cur.substr(idx + 5);
            cur.erase(0, cur.find_first_not_of(' '));
            //std::vector<std::string> words = split(cur, ',');
            std::vector<std::string> words = tokenizer(cur);
            for (int i = 0; i < words.size(); i++) {
                int32_t w = std::stoi(words[i]);
                *get_real_address_from(registers[28], real_memory) = w & 0xff;
                registers[28]++;
            }
            if (registers[28] != 0)
                registers[28] += (4 - (registers[28] % 4));
        } else if (cur.find(".half")!= std::string::npos) {
            idx = cur.find(".half");
            cur = cur.substr(idx + 5,cur.length()+1);
            cur.erase(0, cur.find_first_not_of(' '));
            //std::vector<std::string> words = split(cur, ',');
            std::vector<std::string> words = tokenizer(cur);
            for (int i = 0; i < words.size(); i++) {
                int32_t w = std::stoi(words[i]);
                *get_real_address_from(registers[28], real_memory) = w & 0xff;
                *get_real_address_from(registers[28] + 1, real_memory) = (w % 0xff00)>>8;
                registers[28] += 2;
            }
            if (registers[28] != 0)
                registers[28] += (4 - (registers[28] % 4));
        }
    }

}

void pre_distinguish(unsigned char *PC, _instruction &cur, uint32_t ins) {
    cur.op = (uint32_t) ((ins & 0xfc000000) >> 26);
    switch (cur.op) {
        case 0:
            cur.type = 0;
            cur.rs = (uint32_t) ((ins & 0x3e00000) >> 21);
            cur.rt = (uint32_t) ((ins & 0x1f0000) >> 16);
            cur.rd = ((uint32_t) ((ins & 0xf800)) >> 11);
            cur.shamt = (uint32_t) ((ins & 0x7c0) >> 6);
            cur.funct = (uint32_t) (ins & 0x3f);
            break;
        case 2:
            cur.type = 2;
            cur.target = (uint32_t) (ins & 0x3ffffff);
            break;
        case 3:
            cur.type = 2;
            cur.target = (uint32_t) (ins & 0x3ffffff);
            break;
        case 0x1c:
            cur.type = 0;
            cur.rs = (uint32_t) ((ins & 0x3e00000) >> 21);
            cur.rt = (uint32_t) ((ins & 0x1f0000) >> 16);
            cur.rd = ((uint32_t) ((ins & 0xf800)) >> 11);
            cur.shamt = (uint32_t) ((ins & 0x7c0) >> 6);
            cur.funct = (uint32_t) (ins & 0x3f);
            break;
        default:
            cur.type = 1;
            cur.rs = (uint32_t) ((ins & 0x3e00000) >> 21);
            cur.rt = (uint32_t) ((ins & 0x1f0000) >> 16);
            cur.imme = (int16_t) (ins & 0x0000ffff);
            break;
    }
    return;
}

void perform_code(uint32_t *&registers, unsigned char *&PC, unsigned char *&real_mem, std::string input_path,
                  std::string output_path) {
    _instruction cur;
    std::ifstream input;
    std::ofstream output;
    input.open(input_path);
    output.open(output_path);
    while (true) {
        uint32_t ins =
                (uint32_t) *PC + ((uint32_t) *(PC + 1) << 8) + ((uint32_t) *(PC + 2) << 16) +
                ((uint32_t) *(PC + 3) << 24);
        if (ins == 0) break;
        pre_distinguish(PC, cur, ins);
        PC += 4;
        switch (cur.type) {
            case 0:
                switch (cur.funct) {
                    case 0x20:
                        switch (cur.op) {
                            case 0x1c:
                                _clz(registers, cur.rs, cur.rd);
                                break;
                            case 0:
                                _add(registers, cur.rs, cur.rt, cur.rd,output);
                                break;
                        }
                        break;
                    case 0x21:
                        switch (cur.op) {
                            case 0x1c:
                                _clo(registers, cur.rs, cur.rd);
                                break;
                            case 0:
                                _addu(registers, cur.rs, cur.rt, cur.rd);
                                break;
                        }
                        break;
                    case 0x24:
                        _and(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 0x1a:
                        _div(registers, cur.rs, cur.rt);
                        break;
                    case 0x1b:
                        _divu(registers, cur.rs, cur.rt);
                        break;
                    case 0x18:
                        _mult(registers, cur.rs, cur.rt);
                        break;
                    case 0x19:
                        _multu(registers, cur.rs, cur.rt);
                        break;
                    case 2:
                        switch (cur.op) {
                            case 0x1c:
                                _mul(registers, cur.rs, cur.rt, cur.rd);
                                break;
                            case 0:
                                _srl(registers, cur.rs, cur.rt, cur.shamt);
                                break;

                        }
                        break;
                    case 0:
                        switch (cur.op) {
                            case 0x1c:
                                _madd(registers, cur.rs, cur.rt);
                                break;
                            case 0:
                                _sll(registers, cur.rs, cur.rt, cur.shamt);
                                break;
                        }
                        break;
                    case 1:
                        switch (cur.op) {
                            case 0x1c:
                                _maddu(registers, cur.rs, cur.rt);
                                break;

                        }
                        break;
                    case 4:
                        switch (cur.op) {
                            case 0x1c:
                                _msub(registers, cur.rs, cur.rt);
                                break;
                            case 0:
                                _sllv(registers, cur.rs, cur.rt, cur.rd);
                                break;
                        }
                        break;
                    case 5:
                        switch (cur.op) {
                            case 0x1c:
                                _msubu(registers, cur.rs, cur.rt);
                                break;

                        }
                        break;
                    case 0x27:
                        _nor(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 0x25:
                        _or(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 3:
                        _sra(registers, cur.rs, cur.rt, cur.shamt);
                        break;
                    case 7:
                        _srav(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 6:
                        _srlv(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 0x22:
                        _sub(registers, cur.rs, cur.rt, cur.rd,output);
                        break;
                    case 0x23:
                        _subu(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 0x26:
                        _xor(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 0x2a:
                        _slt(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 0x2b:
                        _sltu(registers, cur.rs, cur.rt, cur.rd);
                        break;
                    case 9:
                        _jalr(registers, cur.rs, cur.rd, PC, real_mem);
                        break;
                    case 8:
                        _jr(registers, cur.rs, PC, real_mem);
                        break;
                    case 0x34:
                        _teq(registers, cur.rs, cur.rt,output);
                        break;
                    case 0x36:
                        _tne(registers, cur.rs, cur.rt,output);
                        break;
                    case 0x30:
                        _tge(registers, cur.rs, cur.rt,output);
                        break;
                    case 0x31:
                        _tgeu(registers, cur.rs, cur.rt,output);
                        break;
                    case 0x32:
                        _tlt(registers, cur.rs, cur.rt,output);
                        break;
                    case 0x33:
                        _tltu(registers, cur.rs, cur.rt,output);
                        break;
                    case 0x10:
                        _mfhi(registers, cur.rd);
                        break;
                    case 0x12:
                        _mflo(registers, cur.rd);
                        break;
                    case 0x11:
                        _mthi(registers, cur.rs);
                        break;
                    case 0x13:
                        _mtlo(registers, cur.rs);
                        break;
                    case 0xc:
                        _syscall(registers,input,output,real_mem);
                        break;
                }
            case 1:
                switch (cur.op) {
                    case 8:
                        _addi(registers, cur.rs, cur.rt, cur.imme,output);
                        break;
                    case 9:
                        _addiu(registers, cur.rs, cur.rt, cur.imme);
                        break;
                    case 0xc:
                        _andi(registers, cur.rs, cur.rt, cur.imme);
                        break;
                    case 0xd:
                        _ori(registers, cur.rs, cur.rt, (uint16_t)cur.imme);
                        break;
                    case 0xe:
                        _xori(registers, cur.rs, cur.rt, cur.imme);
                        break;
                    case 0xf:
                        _lui(registers, cur.rt, cur.imme);
                        break;
                    case 0xa:
                        _slti(registers, cur.rs, cur.rt, cur.imme);
                        break;
                    case 0xb:
                        _sltiu(registers, cur.rs, cur.rt, cur.imme);
                        break;
                    case 4:
                        _beq(registers, cur.rs, cur.rt, cur.imme, PC);
                        break;
                    case 1:
                        switch (cur.rt) {
                            case 1:
                                _bgez(registers, cur.rs, (int16_t) cur.imme, PC);
                                break;
                            case 0x11:
                                _bgezal(registers, cur.rs, (int16_t) cur.imme, PC, real_mem);
                                break;
                            case 0x10:
                                _bltzal(registers, cur.rs, (int16_t) cur.imme, PC, real_mem);
                                break;
                            case 0:
                                _bltz(registers, cur.rs, (int16_t) cur.imme, PC);
                                break;
                            case 0xc:
                                _teqi(registers, cur.rs, cur.imme,output);
                                break;
                            case 0xe:
                                _tnei(registers, cur.rs, cur.imme,output);
                                break;
                            case 8:
                                _tgei(registers, cur.rs, cur.imme,output);
                                break;
                            case 9:
                                _tgeiu(registers, cur.rs, cur.imme,output);
                                break;
                            case 0xa:
                                _tlti(registers, cur.rs, cur.imme,output);
                                break;
                            case 0xb:
                                _tltiu(registers, cur.rs, cur.imme,output);
                                break;
                        }
                        break;
                    case 7:
                        _bgtz(registers, cur.rs, cur.imme, PC);
                        break;
                    case 6:
                        _blez(registers, cur.rs, cur.imme, PC);
                        break;
                    case 5:
                        _bne(registers, cur.rs, cur.rt, cur.imme, PC);
                        break;
                    case 0x20:
                        _lb(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x24:
                        _lbu(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x21:
                        _lh(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x25:
                        _lhu(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x23:
                        _lw(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x22:
                        _lwl(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x26:
                        _lwr(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x30:
                        _ll(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x28:
                        _sb(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x29:
                        _sh(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x2b:
                        _sw(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x2a:
                        _swl(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x2e:
                        _swr(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                    case 0x38:
                        _sc(registers, cur.rs, cur.rt, cur.imme, real_mem);
                        break;
                }
                break;
            case 2:
                switch (cur.op) {
                    case 2:
                        _j(cur.target, PC, real_mem);
                        break;
                    case 3:
                        _jal((int32_t) cur.target, PC, registers, real_mem);
                        break;
                }
                break;
        }
    }
    input.close();
    output.close();
}

std::string string_clear(std::string str) {
    std::string res;
    int idx1;
    int idx2;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '"'){
            idx1 = i;
            break;
        }
    }
    res = str.substr(idx1+1);
    for (int i = 0; i < res.length();i++){
        if (res[i] == 92){
            // '\n'
            if (res[i+1] == 110){
                res[i] = 10;
                res = res.substr(0,i+1)+res.substr(i+2);
            }
            // '\0'
            else if(res[i+1] == 48){
                res[i] = 0;
                res = res.substr(0,i+1)+res.substr(i+2);
            }
            // '\t'
            else if (res[i+1] == 116){
                res[i] = 9;
                res = res.substr(0,i+1)+res.substr(i+2);
            }
            // '\\'
            else if (res[i+1] == 92){
                res = res.substr(0,i+1)+res.substr(i+2);
            }
            // '\"'
            else if (res[i+1] == 34){
                res[i] = 34;
                res = res.substr(0,i+1)+res.substr(i+2);
            }
        }
    }
    for (int i = res.length(); i > 0; i--){
        if (res[i] == 34){
            idx2 = i;
            break;
        }
    }
    return res.substr(0,idx2);
}