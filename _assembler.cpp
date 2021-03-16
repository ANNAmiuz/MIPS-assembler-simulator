#include "_assembler.h"

bool is_alldigit(const std::string &str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

//1 from 2
std::vector<std::string> tokenizer(std::string text) {
    std::replace(text.begin(), text.end(), ',', ' ');
    std::vector<std::string> results;
    std::string result;
    std::stringstream input(text);
    while (input >> result) results.push_back(result);
    return results;
}

std::vector<std::string> split(std::string &s, char delimiter) {
    std::replace(s.begin(), s.end(), ',', ' ');
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string ito_binary(int num, int digit) {
    int bin[32];
    std::string result;
    for (int i = 31; i >= 0; i--) {
        if (num & (1 << i)) {
            bin[30 - i] = 1;
        } else {
            bin[30 - i] = 0;
        }
    }
    for (int j = 0; j < digit; j++) {
        result = std::to_string(bin[30 - j]) + result;
    }
    return result;
}

void reg_mapping(std::map<std::string, uint32_t> &reg_map) {
    reg_map["$zero"] = 0;
    reg_map["$at"] = 1;
    reg_map["$v0"] = 2;
    reg_map["$v1"] = 3;
    reg_map["$a0"] = 4;
    reg_map["$a1"] = 5;
    reg_map["$a2"] = 6;
    reg_map["$a3"] = 7;
    reg_map["$t0"] = 8;
    reg_map["$t1"] = 9;
    reg_map["$t2"] = 10;
    reg_map["$t3"] = 11;
    reg_map["$t4"] = 12;
    reg_map["$t5"] = 13;
    reg_map["$t6"] = 14;
    reg_map["$t7"] = 15;
    reg_map["$t8"] = 24;
    reg_map["$t9"] = 25;
    reg_map["$s0"] = 16;
    reg_map["$s1"] = 17;
    reg_map["$s2"] = 18;
    reg_map["$s3"] = 19;
    reg_map["$s4"] = 20;
    reg_map["$s5"] = 21;
    reg_map["$s6"] = 22;
    reg_map["$s7"] = 23;
    reg_map["$k0"] = 26;
    reg_map["$k1"] = 27;
    reg_map["$gp"] = 28;
    reg_map["$sp"] = 29;
    reg_map["$fp"] = 30;
    reg_map["$ra"] = 31;
}

void opr_mapping(std::map<std::string, Operation> &opr) {
    opr["add"] = Add;
    opr["addu"] = Addu;
    opr["addi"] = Addi;
    opr["addiu"] = Addiu;
    opr["and"] = And;
    opr["andi"] = Andi;
    opr["clo"] = Clo;
    opr["clz"] = Clz;
    opr["div"] = Div;
    opr["divu"] = Divu;
    opr["mult"] = Mult;
    opr["multu"] = Multu;
    opr["mul"] = Mul;
    opr["madd"] = Madd;
    opr["msub"] = Msub;
    opr["maddu"] = Maddu;
    opr["msubu"] = Msubu;
    opr["nor"] = Nor;
    opr["or"] = Or;
    opr["ori"] = Ori;
    opr["sll"] = Sll;
    opr["sllv"] = Sllv;
    opr["sra"] = Sra;
    opr["srav"] = Srav;
    opr["srl"] = Srl;
    opr["srlv"] = Srlv;
    opr["sub"] = Sub;
    opr["subu"] = Subu;
    opr["xor"] = Xor;
    opr["xori"] = Xori;
    opr["lui"] = Lui;
    opr["slt"] = Slt;
    opr["sltu"] = Sltu;
    opr["slti"] = Slti;
    opr["sltiu"] = Sltiu;
    opr["beq"] = Beq;
    opr["bgez"] = Bgez;
    opr["bgezal"] = Bgezal;
    opr["bgtz"] = Bgtz;
    opr["blez"] = Blez;
    opr["bltzal"] = Bltzal;
    opr["bltz"] = Bltz;
    opr["bne"] = Bne;
    opr["j"] = J;
    opr["jal"] = Jal;
    opr["jalr"] = Jalr;
    opr["jr"] = Jr;
    opr["teq"] = Teq;
    opr["teqi"] = Teqi;
    opr["tne"] = Tne;
    opr["tnei"] = Tnei;
    opr["tge"] = Tge;
    opr["tge"] = Tge;
    opr["tgeu"] = Tgeu;
    opr["tgei"] = Tgei;
    opr["tgeiu"] = Tgeiu;
    opr["tlt"] = Tlt;
    opr["tltu"] = Tltu;
    opr["tlti"] = Tlti;
    opr["tltiu"] = Tltiu;
    opr["lb"] = Lb;
    opr["lbu"] = Lbu;
    opr["lh"] = Lh;
    opr["lhu"] = Lhu;
    opr["lw"] = Lw;
    opr["lwl"] = Lwl;
    opr["lwr"] = Lwr;
    opr["ll"] = Ll;
    opr["sb"] = Sb;
    opr["sh"] = Sh;
    opr["sw"] = Sw;
    opr["swl"] = Swl;
    opr["swr"] = Swr;
    opr["sc"] = Sc;
    opr["mfhi"] = Mfhi;
    opr["mflo"] = Mflo;
    opr["mthi"] = Mthi;
    opr["mtlo"] = Mtlo;
    opr["syscall"] = Syscall;
}

void scan_process(std::map<std::string, Operation> opr_map, std::map<std::string, uint32_t> reg_map,
                  std::map<std::string, unsigned int> &symbol_table, std::vector<Instruction *> &MIPS_binary,
                  std::vector<std::string> &MIPS_data,
                  uint32_t base_address, std::string _asm) {
    std::ifstream infile;
    infile.open(_asm);
    std::vector<std::string> MIPS_text;
    std::string current;
    std::string symbol;
    int text_flag = 0;
    int data_flag = 0;
    //read 1st time
    //delete all the comments, label at the head, space at the two ends
    //store the instruction into 2 vector: MIPS data & text
    //maintain the symbol table
    while (getline(infile, current)) {
        if (current.find("#") != std::string::npos)
            current.erase(current.find("#"), current.length() + 1);
        if (!current.empty()) {
            if (current == ".data") {
                data_flag = 1;
                text_flag = 0;
                continue;
            } else if (current == ".text") {
                data_flag = 0;
                text_flag = 1;
                continue;
            }
            if (data_flag == 1 && text_flag == 0) {
                MIPS_data.push_back(current);
                continue;
            }
            if (text_flag == 1 && data_flag == 0 && current.find(':') != std::string::npos) {
                symbol = current.substr(current.find_first_not_of(" "),
                                         current.find(":")-current.find_first_not_of(" "));
                current.erase(0, current.find(":") + 1);
                current.erase(0,current.find_first_not_of(' '));
                if (current.empty()) continue;
            }
            if (text_flag == 1 && data_flag == 0) {
                MIPS_text.push_back(current);
                if (!symbol.empty()) {
                    symbol_table[symbol] = MIPS_text.size()-1;
                    symbol = "";
                }
            }
        }
    }
    //
    //for(std::map<std::string, unsigned int>::iterator iter = symbol_table.begin(); iter != symbol_table.end(); iter++) {
        //std::cout << iter->first << " : " << iter->second << std::endl;
    //}
    //read the 2nd time from the vector
    for (int i = 0; i < MIPS_text.size(); i++) {
        translate(MIPS_text[i], i, opr_map, reg_map, MIPS_binary, symbol_table, base_address);
    }
    infile.close();
}

void translate(std::string instruction, int PC, std::map<std::string, Operation> opr_map,
               std::map<std::string, uint32_t> reg_map, std::vector<Instruction *> &MIPS_binary,
               std::map<std::string, unsigned int> symbol_table, int base_address) {
    //a
    //std::cout << instruction << std::endl;
    std::vector<std::string> pieces = tokenizer(instruction);
    //a
    //std::cout << PC;
    //a
    //for (int i = 0; i < pieces.size(); i++) {
        //std::cout << pieces[i] << " ";
    //}
    //a
    //std::cout << std::endl;
    //Instruction cur;
    Instruction *cur = new Instruction;
    if (pieces.empty()) return;
    switch (opr_map[pieces[0]]) {
        case Add:
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[3]];
            cur->rd = reg_map[pieces[1]];
            cur->shamt = 0;
            cur->funct = 0x20;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Addu:
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[3]];
            cur->rd = reg_map[pieces[1]];
            cur->shamt = 0;
            cur->funct = 0x21;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Addi:
            cur->op = 8;
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Addiu:
            cur->op = 9;
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case And:
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[3]];
            cur->rd = reg_map[pieces[1]];
            cur->shamt = 0;
            cur->funct = 0x24;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Andi:

            cur->op = 0xc;
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Clo:
            cur->op = 0x1c;
            cur->rs = reg_map[pieces[2]];
            cur->rd = reg_map[pieces[1]];
            cur->shamt = 0;
            cur->funct = 0x21;
            cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "00000" + ito_binary(cur->rd, 5) +
                                ito_binary(cur->shamt, 5) +
                                ito_binary(cur->funct, 6);
            break;
        case Clz:
            cur->op = 0x1c;
            cur->rs = reg_map[pieces[2]];
            cur->rd = reg_map[pieces[1]];
            cur->shamt = 0;
            cur->funct = 0x20;
            cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "00000" + ito_binary(cur->rd, 5) +
                                ito_binary(cur->shamt, 5) +
                                ito_binary(cur->funct, 6);
            break;
        case Div:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x1a;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Divu:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x1b;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Mult:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x18;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Multu:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x19;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Mul:
            cur->rd = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[3]];
            cur->shamt = 0;
            cur->funct = 0x2;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Madd:
            cur->op = 0x1c;
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Msub:
            cur->op = 0x1c;
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 4;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Maddu:
            cur->op = 0x1c;
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 1;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Msubu:
            cur->op = 0x1c;
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 5;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + "0000000000" +
                    ito_binary(cur->funct, 6);
            break;
        case Nor:
            cur->rd = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[3]];
            cur->shamt = 0;
            cur->funct = 0x27;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Or:
            cur->rd = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->rt = reg_map[pieces[3]];
            cur->shamt = 0;
            cur->funct = 0x25;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Ori:
            cur->op = 0xd;
            cur->rt = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Sll:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->shamt = std::stoi(pieces[3]);
            cur->funct = 0;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Sllv:
            cur->rd = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[3]];
            cur->rt = reg_map[pieces[2]];
            cur->shamt = 0;
            cur->funct = 4;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Sra:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->shamt = std::stoi(pieces[3]);
            cur->funct = 3;
            cur->machine_code = ito_binary(cur->op, 6) + "00000" + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                                ito_binary(cur->shamt, 5) +
                                ito_binary(cur->funct, 6);
            break;
        case Srav:
            cur->rd = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[3]];
            cur->rt = reg_map[pieces[2]];
            cur->shamt = 0;
            cur->funct = 7;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Srl:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->shamt = std::stoi(pieces[3]);
            cur->funct = 2;
            cur->machine_code = ito_binary(cur->op, 6) + "00000" + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                                ito_binary(cur->shamt, 5) +
                                ito_binary(cur->funct, 6);
            break;
        case Srlv:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->rs = reg_map[pieces[3]];
            cur->funct = 6;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Sub:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[3]];
            cur->rs = reg_map[pieces[2]];
            cur->funct = 0x22;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Subu:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[3]];
            cur->rs = reg_map[pieces[2]];
            cur->funct = 0x23;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Xor:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[3]];
            cur->rs = reg_map[pieces[2]];
            cur->funct = 0x26;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Xori:
            cur->op = 0xe;
            cur->rt = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lui:
            cur->op = 0xf;
            cur->rt = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code = ito_binary(cur->op, 6) + "00000" + ito_binary(cur->rt, 5) + ito_binary(cur->imme, 16);
            break;
        case Slt:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[3]];
            cur->rs = reg_map[pieces[2]];
            cur->funct = 0x2a;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Sltu:
            cur->rd = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[3]];
            cur->rs = reg_map[pieces[2]];
            cur->funct = 0x2b;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Slti:
            cur->op = 0xa;
            cur->rt = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Sltiu:
            cur->op = 0xb;
            cur->rt = reg_map[pieces[1]];
            cur->rs = reg_map[pieces[2]];
            cur->imme = std::stoi(pieces[3]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Beq:
            cur->op = 4;
            cur->rt = reg_map[pieces[2]];
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[3];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, 0, 0), 16);
            }
            cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                ito_binary(symbol_table[cur->target] - PC - 1, 16);
            break;
        case Bgez:
            cur->op = 1;
            cur->rt = 1;
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, 0, 0), 16);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "00001" +
                    ito_binary(symbol_table[cur->target] - PC - 1, 16);
            break;
        case Bgezal:
            cur->op = 1;
            cur->rt = 0x11;
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, 0, 0), 16);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "10001" +
                    ito_binary(symbol_table[cur->target] - PC - 1, 16);
            break;
        case Bgtz:
            cur->op = 7;
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, 0, 0), 16);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "00000" +
                    ito_binary(symbol_table[cur->target]-PC-1, 16);
            break;
        case Blez:
            cur->op = 6;
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, nullptr, 0), 16);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt,5) +
                    ito_binary(symbol_table[cur->target]-PC-1, 16);
            break;
        case Bltzal:
            cur->op = 1;
            cur->rt = 0x10;
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, nullptr, 0), 16);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "10000" +
                    ito_binary(symbol_table[cur->target]-PC-1, 16);
            break;
        case Bltz:
            cur->op = 1;
            cur->rt = 0;
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, nullptr, 0), 16);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "00000" +
                    ito_binary(symbol_table[cur->target]-PC-1, 16);
            break;
        case Bne:
            cur->op = 5;
            cur->rt = reg_map[pieces[2]];
            cur->rs = reg_map[pieces[1]];
            cur->target = pieces[3];
            if (is_alldigit(cur->target)) {
                cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                    ito_binary(std::stoi(cur->target, nullptr, 0), 16);
            }
            cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                                ito_binary(symbol_table[cur->target] - PC-1, 16);
            break;
            //how to get the 26-bit address
        case J:
            cur->op = 2;
            cur->target = pieces[1];
            if (is_alldigit(cur->target)) {
                cur->machine_code =
                        ito_binary(cur->op, 6) + ito_binary((std::stoi(cur->target, nullptr, 0) << 4) >> 6, 26);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(((4 * symbol_table[cur->target] + base_address) << 4) >> 6, 26);
            break;
            //How to get the 26-bit address
        case Jal:
            cur->op = 3;
            cur->target = pieces[1];
            if (is_alldigit(cur->target)) {
                cur->machine_code =
                        ito_binary(cur->op, 6) + ito_binary((std::stoi(cur->target, nullptr, 0) << 4) >> 6, 26);
            }
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(((4 * symbol_table[cur->target] + base_address) << 4) >> 6, 26);
            break;
        case Jalr:
            cur->rs = reg_map[pieces[1]];
            cur->rd = reg_map[pieces[2]];
            cur->funct = 9;
            cur->machine_code = ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "00000" + ito_binary(cur->rd, 5) +
                                ito_binary(cur->shamt, 5) +
                                ito_binary(cur->funct, 6);
            break;
        case Jr:
            cur->rs = reg_map[pieces[1]];
            cur->funct = 8;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + "000000000000000" + ito_binary(cur->funct, 6);
            break;
        case Teq:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x34;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Teqi:
            cur->op = 1;
            cur->rt = 0xc;
            cur->rs = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Tne:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x36;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Tnei:
            cur->op = 1;
            cur->rt = 0xe;
            cur->rs = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Tge:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x30;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
        case Tgeu:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x31;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Tgei:

            cur->op = 1;
            cur->rt = 8;
            cur->rs = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Tgeiu:

            cur->op = 1;
            cur->rt = 9;
            cur->rs = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rs, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Tlt:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x32;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Tltu:
            cur->rs = reg_map[pieces[1]];
            cur->rt = reg_map[pieces[2]];
            cur->funct = 0x33;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Tlti:

            cur->op = 1;
            cur->rt = 0xa;
            cur->rs = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Tltiu:

            cur->op = 1;
            cur->rt = 0xb;
            cur->rs = reg_map[pieces[1]];
            cur->imme = std::stoi(pieces[2]);
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
            //how to determine the address
        case Lb:

            cur->op = 0x20;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lbu:

            cur->op = 0x24;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lh:
            cur->op = 0x21;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lhu:
            cur->op = 0x25;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lw:
            cur->op = 0x23;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lwl:
            cur->op = 0x22;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Lwr:
            cur->op = 0x26;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Ll:
            cur->op = 0x30;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Sb:
            cur->op = 0x28;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Sh:
            cur->op = 0x29;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Sw:
            cur->op = 0x2b;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Swl:
            cur->op = 0x2a;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Swr:
            cur->op = 0x2e;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Sc:
            cur->op = 0x38;
            cur->rt = reg_map[pieces[1]];
            cur->target = pieces[2];
            if (cur->target.substr(0, cur->target.find_first_of("(")) == "") {
                cur->imme = 0;
            } else { cur->imme = std::stoi(cur->target.substr(0, cur->target.find_first_of("("))); }
            cur->rs = reg_map[cur->target.substr(cur->target.find_first_of("(") + 1, 3)];
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) +
                    ito_binary(cur->imme, 16);
            break;
        case Mfhi:
            cur->rd = reg_map[pieces[1]];
            cur->funct = 0x10;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Mflo:
            cur->rd = reg_map[pieces[1]];
            cur->funct = 0x12;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Mthi:
            cur->rs = reg_map[pieces[1]];
            cur->funct = 0x11;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Mtlo:
            cur->rs = reg_map[pieces[1]];
            cur->funct = 0x13;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
        case Syscall:
            cur->funct = 0xc;
            cur->machine_code =
                    ito_binary(cur->op, 6) + ito_binary(cur->rs, 5) + ito_binary(cur->rt, 5) + ito_binary(cur->rd, 5) +
                    ito_binary(cur->shamt, 5) +
                    ito_binary(cur->funct, 6);
            break;
    }
    MIPS_binary.push_back(cur);

    //a&s
    //std::cout << cur->machine_code << std::endl;
}


void test(std::string out, std::vector<Instruction *> &MIPS_binary) {
    std::fstream infile(out);
    std::string res;
    for (int i = 0; i < MIPS_binary.size(); i++) {
        std::getline(infile, res);
        if (res != MIPS_binary[i]->machine_code)
            std::cout << i << std::endl;
    }
}