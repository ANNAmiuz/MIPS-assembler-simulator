#ifndef CSC3050PRO1_MIPS_OPR_H
#define CSC3050PRO1_MIPS_OPR_H

/*actual operation realization of all 79 mips instructions*/

uint32_t get_simulated_address_from(unsigned char *real_address, unsigned char *real_memory) {
    uint32_t result = real_address - real_memory + 0x400000;
    return result;
}

unsigned char *get_real_address_from(uint32_t simulated_address, unsigned char *real_memory) {
    unsigned char *result = real_memory + (simulated_address - 0x400000);
    return result;
}

/*
 * Perform branch opr with PC and offset
 */
void branch(unsigned char *&PC, int16_t offset) {
    int32_t off = (int32_t) offset;
    off = off << 2;
    PC += off;
}

void _add(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd, std::ofstream &out) {
    int32_t res = (int32_t) registers[rs] + (int32_t) registers[rt];
    if ((0x80000000 & registers[rs]) != (0x80000000 & res)) {
        if ((0x80000000 & registers[rs]) == (0x80000000 & registers[rt])) {
            out << "Integer_Overflow";
            std::exit(EXIT_FAILURE);
        }
    }
    registers[rd] = (uint32_t) res;
}

void _addu(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rs] + registers[rt];
}

void _addi(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t i, std::ofstream &out) {
    //std::cout<<(int32_t) registers[rs]<<"+" <<(int32_t) i;
    int32_t res = (int32_t) registers[rs] + (int32_t) i;
    if (((0x80000000 & registers[rs]) == (0x80000000 & (int32_t) i)) &&
        ((0x80000000 & registers[rs]) != (0x80000000 & res))) {
        out << "Integer_Overflow";
        std::exit(EXIT_FAILURE);
    }
    registers[rt] = (uint32_t) res;
}

void _addiu(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t i) {
    registers[rt] = registers[rs] + ((int32_t) i);
}

void _and(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rt] & registers[rs];
}

void _andi(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t i) {
    registers[rt] = registers[rs] & ((uint16_t) i);
}

void _clo(uint32_t *&registers, uint32_t rs, uint32_t rd) {
    uint32_t result = 0;
    for (int i = 31; i >= 0; i--) {
        if (registers[rs] & (1u << i)) result++;
        else break;
    }
    registers[rd] = result;
}

void _clz(uint32_t *&registers, uint32_t rs, uint32_t rd) {
    uint32_t result = 0;
    for (int i = 31; i >= 0; i--) {
        if (registers[rs] & (1u << i)) break;
        else result++;
    }
    registers[rd] = result;
}

void _div(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    int32_t quotient = (int32_t) registers[rs] / (int32_t) registers[rt];
    int32_t remainder = (int32_t) registers[rs] % (int32_t) registers[rt];
    registers[32] = (uint32_t) quotient;
    registers[33] = (uint32_t) remainder;
}

void _divu(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    uint32_t quotient = registers[rs] / registers[rt];
    uint32_t remainder = registers[rs] % registers[rt];
    registers[32] = quotient;
    registers[33] = remainder;
}

void _mult(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    int32_t x = (int32_t) registers[rs];
    int32_t y = (int32_t) registers[rt];
    int64_t result = (int64_t) x * (int64_t) y;
    registers[32] = (uint32_t) (result & 0xffff);
    registers[33] = (uint32_t) (result & 0xffff0000);
}

void _multu(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    uint64_t result = (uint64_t) registers[rs] * (uint64_t) registers[rt];
    registers[32] = result & 0xffff;
    registers[33] = result & 0xffff0000;
}

void _mul(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    int32_t x = (int32_t) registers[rs];
    int32_t y = (int32_t) registers[rt];
    int64_t res = (int64_t) x * (int64_t) y;
    registers[rd] = (uint32_t) (res & 0xffff);
}

void _madd(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    int32_t x = (int32_t) registers[rs];
    int32_t y = (int32_t) registers[rt];
    int64_t res = (int64_t) x * (int64_t) y;
    int32_t lo = (int32_t) registers[32];
    int32_t hi = (int32_t) registers[33];
    res += (int64_t) lo + ((int64_t) hi << 32);
    registers[32] = (uint32_t) (res & 0xffff);
    registers[33] = (uint32_t) (res & 0xffff0000);
}

void _maddu(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    uint64_t result = (uint64_t) registers[rs] * (uint64_t) registers[rt];
    result = result + ((uint64_t) registers[32]) + (((uint64_t) registers[33]) << 32);
    registers[32] = (uint32_t) (result & 0xffff);
    registers[33] = (uint32_t) (result & 0xffff0000);
}

void _msub(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    int32_t x = (int32_t) registers[rs];
    int32_t y = (int32_t) registers[rt];
    int64_t res = (int64_t) x * (int64_t) y;
    res = ((int64_t) (int32_t) registers[32]) + (((int64_t) (int32_t) registers[33]) << 32) - res;
    registers[32] = (uint32_t) (res & 0xffff);
    registers[33] = (uint32_t) (res & 0xffff0000);
}

void _msubu(uint32_t *&registers, uint32_t rs, uint32_t rt) {
    uint64_t result = (uint64_t) registers[rs] * (uint64_t) registers[rt];
    result = ((uint64_t) registers[32]) + (((uint64_t) registers[33]) << 32) - result;
    registers[32] = (uint32_t) (result & 0xffff);
    registers[33] = (uint32_t) (result & 0xffff0000);
}

void _nor(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = ~(registers[rs] | registers[rt]);
}

void _or(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rs] | registers[rt];
}

void _ori(uint32_t *&registers, uint32_t rs, uint32_t rt, uint16_t i) {
    registers[rt] = registers[rs] | (uint32_t) i;}

void _sll(uint32_t *&registers, uint32_t rd, uint32_t rt, uint32_t shamt) {
    registers[rd] = registers[rt] << shamt;
    return;
}

void _sllv(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rt] << registers[rs];
    return;
}

void _sra(uint32_t *&registers, uint32_t rd, uint32_t rt, uint32_t shamt) {
    int32_t x = (int32_t) registers[rt];
    int32_t y = x >> shamt;
    registers[rd] = (uint32_t) y;
    return;
}

void _srav(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    int32_t x = (int32_t) registers[rt];
    int32_t y = x >> (int32_t) registers[rs];
    registers[rd] = (uint32_t) y;
    return;
}

void _srl(uint32_t *&registers, uint32_t rd, uint32_t rt, uint32_t shamt) {
    registers[rd] = registers[rt] >> shamt;
    return;
}

void _srlv(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rt] >> registers[rs];
    return;
}

void _sub(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd, std::ofstream &out) {
    int32_t res = (int32_t) registers[rs] - (int32_t) registers[rt];
    if (((0x80000000 & registers[rs]) != (0x80000000 & registers[rt])) &&
        ((0x80000000 & registers[rs]) != (0x80000000 & res))) {
        out << "Overflow";
        std::exit(EXIT_FAILURE);
    }
    registers[rd] = (uint32_t) (res);
}

void _subu(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rs] - registers[rt];
    return;
}

void _xor(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    registers[rd] = registers[rs] ^ registers[rt];
    return;
}

void _xori(uint32_t *&registers, uint32_t rs, uint32_t rt, uint16_t i) {
    registers[rt] = registers[rs] ^ ((uint32_t) i);
    return;
}

void _lui(uint32_t *&registers, uint32_t rt, uint16_t i) {
    uint32_t x = (uint32_t) i;
    registers[rt] = (uint32_t) (x << 16);return;
}

void _slt(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    if ((int32_t) registers[rs] < (int32_t) registers[rt]) registers[rd] = 1;
    else registers[rd] = 0;
    return;
}

void _sltu(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd) {
    if (registers[rs] < registers[rt]) registers[rd] = 1;
    else registers[rd] = 0;
    return;
}

void _slti(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t i) {
    if ((int32_t) registers[rs] < (int32_t) i) registers[rt] = 1;
    else registers[rt] = 0;
    return;
}

void _sltiu(uint32_t *&registers, uint32_t rs, uint32_t rt, uint16_t i) {
    if (registers[rs] < (uint32_t) ((int32_t) i)) registers[rt] = 1;
    else registers[rt] = 0;
    return;
}

void _beq(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&PC) {
    if (registers[rs] == registers[rt]) branch(PC, offset);
    return;
}

void _bgez(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC) {
    if ((int32_t) registers[rs] >= 0) branch(PC, offset);
    return;
}

void _bgezal(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC, unsigned char *&real_memory) {
    if ((int32_t) registers[rs] >= 0) {
        branch(PC, offset);
        registers[31] = get_simulated_address_from(PC, real_memory);
    }
    return;
}

void _bgtz(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC) {
    if ((int32_t) registers[rs] > 0) branch(PC, offset);
    return;
}

void _blez(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC) {
    if (((int32_t) registers[rs]) <= 0) {
        branch(PC, offset);
    }
    return;
}

void _blezal(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC, unsigned char *&real_memory) {
    if ((int32_t) registers[rs] <= 0) {
        branch(PC, offset);
        registers[31] = get_simulated_address_from(PC, real_memory);
    }
    return;
}

void _bltz(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC) {
    if ((int32_t) registers[rs] < 0) {
        branch(PC, offset);
    }
    return;
}

void _bltzal(uint32_t *&registers, uint32_t rs, int16_t offset, unsigned char *&PC, unsigned char *&real_memory) {
    if ((int32_t) registers[rs] < 0) {
        branch(PC, offset);
        registers[31] = get_simulated_address_from(PC, real_memory);
    }
    return;
}

void _bne(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&PC) {
    if (registers[rs] != registers[rt]) {
        branch(PC, offset);
    }
    return;
}

void _j(uint32_t target, unsigned char *&PC, unsigned char *&real_memory) {
    int32_t fake_address = ((int32_t) (get_simulated_address_from(PC, real_memory) & 0xf0000000)) + (target << 2);
    PC = get_real_address_from(fake_address, real_memory);
    return;
}

void _jal(uint32_t target, unsigned char *&PC, uint32_t *&registers, unsigned char *&real_memory) {
    registers[31] = get_simulated_address_from(PC, real_memory);
    int32_t fake_address =
            ((int32_t) (get_simulated_address_from(PC, real_memory)) & 0xf0000000) + ((int32_t) target << 2);
    PC = get_real_address_from(fake_address, real_memory);
    return;
}

void _jalr(uint32_t *&registers, uint32_t rs, uint32_t rd, unsigned char *&PC, unsigned char *&real_memory) {
    registers[rd] = get_simulated_address_from(PC, real_memory);
    PC = get_real_address_from(registers[rs], real_memory);
    return;
}

void _jr(uint32_t *&registers, uint32_t rs, unsigned char *&PC, unsigned char *&real_memory) {
    PC = get_real_address_from(registers[rs], real_memory);
    return;
}

void _teq(uint32_t *&registers, uint32_t rs, uint32_t rt, std::ofstream &out) {
    if ((int32_t) registers[rs] == (int32_t) registers[rt]) {
        //throw "rs==rt";
        out << "rs == rt";
        std::exit(EXIT_FAILURE);
    }
    return;
}

void _teqi(uint32_t *&registers, uint32_t rs, int16_t i, std::ofstream &out) {
    if ((int32_t) registers[rs] == (int32_t) i) {
        //throw "rs==immediate";
        out << "rs == immediate";
        std::exit(EXIT_FAILURE);
    }
    return;
}

void _tne(uint32_t *&registers, uint32_t rs, uint32_t rt, std::ofstream &out) {
    if (registers[rs] != registers[rt]) {
        //throw "rs!=rt";
        out << "rs != rt";
        std::exit(EXIT_FAILURE);
    }
}

void _tnei(uint32_t *&registers, uint32_t rs, int16_t i, std::ofstream &out) {
    if ((int32_t) registers[rs] != (int32_t) i) {
        //throw "NOT EQUAL";
        out << "rs != immediate";
        std::exit(EXIT_FAILURE);
    }
}

void _tge(uint32_t *&registers, uint32_t rs, uint32_t rt, std::ofstream &out) {
    if ((int32_t) registers[rs] >= (int32_t) registers[rt]) {
        //throw "GREATER / EQUAL";
        out << "rs >= rt";
        std::exit(EXIT_FAILURE);
    }
}

void _tgeu(uint32_t *&registers, uint32_t rs, uint32_t rt, std::ofstream &out) {
    if (registers[rs] >= registers[rt]) {
        out << "rs >= rt";
        std::exit(EXIT_FAILURE);
    }

}

void _tgei(uint32_t *&registers, uint32_t rs, int16_t i, std::ofstream &out) {
    if ((int32_t) registers[rs] >= (int32_t) i) {
        //throw "GREATER / EQUAL";
        out << "rs >= immediate";
        std::exit(EXIT_FAILURE);
    }
    return;
}

void _tgeiu(uint32_t *&registers, uint32_t rs, int16_t i, std::ofstream &out) {
    if (registers[rs] >= (uint32_t) ((int32_t) i)) {
        //throw "GREATER / EQUAL";
        out << "rs >= immediate";
        std::exit(EXIT_FAILURE);
    }
    return;
}

void _tlt(uint32_t *&registers, uint32_t rs, uint32_t rt, std::ofstream &out) {
    if ((int32_t) registers[rs] < (int32_t) registers[rt]) {
        out << "rs < rt";
        std::exit(EXIT_FAILURE);
    }
}

void _tltu(uint32_t *&registers, uint32_t rs, uint32_t rt, std::ofstream &out) {
    if (registers[rs] < registers[rt]) {
        out << "rs < rt";
        std::exit(EXIT_FAILURE);
    }
}

void _tlti(uint32_t *&registers, uint32_t rs, int16_t i, std::ofstream &out) {
    if ((int32_t) (registers[rs]) < (int32_t) i) {
        out << "rs < immediate";
        std::exit(EXIT_FAILURE);
    }
}

void _tltiu(uint32_t *&registers, uint32_t rs, int16_t i, std::ofstream &out) {
    if ((uint32_t) (registers[rs]) < (uint32_t) ((int32_t) i)) {
        out << "rs < immediate";
        std::exit(EXIT_FAILURE);
    }
    return;
}

void _lb(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    int8_t res = (int8_t) *get_real_address_from(fake_address, real_memory);
    registers[rt] = (int32_t) res;
    return;
}

void _lbu(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    registers[rt] = (uint32_t) ((uint8_t) *get_real_address_from(fake_address, real_memory));
    return;
}

//0123:0's address is the smallest
void _lh(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 1 != 0) {
        throw "not aligned";
    }
    uint16_t first = (uint16_t) (uint8_t) *get_real_address_from(fake_address, real_memory);
    uint16_t second = (uint16_t) (uint8_t) *get_real_address_from(fake_address + 1, real_memory);
    uint16_t res = first + (second << 8);
    registers[rt] = (int32_t) ((int16_t) (res));
    return;
}

void _lhu(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 1 != 0) {
        throw "not aligned";
    }
    uint16_t first = (uint16_t) (uint8_t) *get_real_address_from(fake_address, real_memory);
    uint16_t second = (uint16_t) (uint8_t) *get_real_address_from(fake_address + 1, real_memory);
    uint16_t res = first + (second << 8);
    registers[rt] = (uint32_t) res;
    return;
}

void _lw(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 3 != 0) {
        throw "not aligned";
    }
    uint32_t first = (uint32_t) *get_real_address_from(fake_address, real_memory);
    uint32_t second = (uint32_t) *get_real_address_from(fake_address + 1, real_memory);
    uint32_t third = (uint32_t) *get_real_address_from(fake_address + 2, real_memory);
    uint32_t fourth = (uint32_t) *get_real_address_from(fake_address + 3, real_memory);
    uint32_t res = first + (second << 8) + (third << 16) + (fourth << 24);
    registers[rt] = res;
    return;
}

void _lwl(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    uint32_t res = 0;
    switch (fake_address % 4) {
        case 0:
            res = ((uint32_t) *get_real_address_from(fake_address, real_memory)) << 24;
            registers[rt] = (registers[rt] & 0x00ffffff) + res;
            break;
        case 1:
            res = (((uint32_t) *get_real_address_from(fake_address, real_memory)) << 24) +
                  (((uint32_t) *get_real_address_from(fake_address - 1, real_memory)) << 16);
            registers[rt] = (registers[rt] & 0xffff) + res;
            break;
        case 2:
            res = (((uint32_t) *get_real_address_from(fake_address, real_memory)) << 24) +
                  (((uint32_t) *get_real_address_from(fake_address - 1, real_memory)) << 16) +
                  (((uint32_t) *get_real_address_from(fake_address - 2, real_memory)) << 8);
            registers[rt] = (registers[rt] & 0xff) + res;
            break;
        case 3:
            res = (((uint32_t) *get_real_address_from(fake_address, real_memory)) << 24) +
                  (((uint32_t) *get_real_address_from(fake_address - 1, real_memory)) << 16) +
                  (((uint32_t) *get_real_address_from(fake_address - 2, real_memory)) << 8) +
                  ((uint32_t) *get_real_address_from(fake_address - 3, real_memory));
            registers[rt] = res;
            break;
    }
}

void _lwr(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    uint32_t res = 0;
    switch (fake_address % 4) {
        case 0:
            res = (((uint32_t) *get_real_address_from(fake_address + 3, real_memory)) << 24) +
                  (((uint32_t) *get_real_address_from(fake_address + 2, real_memory)) << 16) +
                  (((uint32_t) *get_real_address_from(fake_address + 1, real_memory)) << 8) +
                  ((uint32_t) *get_real_address_from(fake_address, real_memory));
            registers[rt] = res;
            break;
        case 1:
            res = (((uint32_t) *get_real_address_from(fake_address + 2, real_memory)) << 16) +
                  (((uint32_t) *get_real_address_from(fake_address + 1, real_memory)) << 8) +
                  ((uint32_t) *get_real_address_from(fake_address, real_memory));
            registers[rt] = (registers[rt] & 0xff000000) + res;
            break;
        case 2:
            res = (((uint32_t) *get_real_address_from(fake_address + 1, real_memory)) << 8) +
                  ((uint32_t) *get_real_address_from(fake_address, real_memory));
            registers[rt] = (registers[rt] & 0xffff0000) + res;
            break;
        case 3:
            res = ((uint32_t) *get_real_address_from(fake_address, real_memory));
            registers[rt] = (registers[rt] & 0xffffff00) + res;
            break;

    }
}

void _ll(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 3 != 0) {
        throw "not aligned";
    }
    uint32_t first = (uint32_t) *get_real_address_from(fake_address, real_memory);
    uint32_t second = (uint32_t) *get_real_address_from(fake_address + 1, real_memory);
    uint32_t third = (uint32_t) *get_real_address_from(fake_address + 2, real_memory);
    uint32_t fourth = (uint32_t) *get_real_address_from(fake_address + 3, real_memory);
    uint32_t res = first + (second << 8) + (third << 16) + (fourth << 24);
    registers[rt] = res;
    return;
}

void _sb(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    *get_real_address_from(fake_address, real_memory) = (uint8_t) (registers[rt] & 0xff);
    return;
}

void _sh(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 1 != 0) {
        throw "not aligned";
    }
    *get_real_address_from(fake_address, real_memory) = (uint8_t) (registers[rt] & 0xff);
    *get_real_address_from(fake_address + 1, real_memory) = (uint8_t) ((registers[rt] & 0xff00) >> 8);
    return;
}

void _sw(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 3 != 0) {
        throw "not aligned";
    }
    *get_real_address_from(fake_address + 3, real_memory) = (uint8_t) ((registers[rt] & 0xff000000) >> 24);
    *get_real_address_from(fake_address + 2, real_memory) = (uint8_t) ((registers[rt] & 0xff0000) >> 16);
    *get_real_address_from(fake_address + 1, real_memory) = (uint8_t) ((registers[rt] & 0xff00) >> 8);
    *get_real_address_from(fake_address, real_memory) = (uint8_t) (registers[rt] & 0xff);
    return;
}

void _swl(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    switch (fake_address % 4) {
        case 3:
            *get_real_address_from(fake_address, real_memory) = ((registers[rt] & 0xff000000) >> 24);
            *get_real_address_from(fake_address - 1, real_memory) = ((registers[rt] & 0xff0000) >> 16);
            *get_real_address_from(fake_address - 2, real_memory) = ((registers[rt] & 0xff00) >> 8);
            *get_real_address_from(fake_address - 3, real_memory) = (registers[rt] & 0xff);
            break;
        case 2:
            *get_real_address_from(fake_address, real_memory) = ((registers[rt] & 0xff000000) >> 24);
            *get_real_address_from(fake_address - 1, real_memory) = ((registers[rt] & 0xff0000) >> 16);
            *get_real_address_from(fake_address - 2, real_memory) = ((registers[rt] & 0xff00) >> 8);
            break;
        case 1:
            *get_real_address_from(fake_address, real_memory) = ((registers[rt] & 0xff000000) >> 24);
            *get_real_address_from(fake_address - 1, real_memory) = ((registers[rt] & 0xff0000) >> 16);
            break;
        case 0:
            *get_real_address_from(fake_address, real_memory) = ((registers[rt] & 0xff000000) >> 24);
            break;
    }
    return;
}

void _swr(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    switch (fake_address % 4) {
        case 3:
            *get_real_address_from(fake_address, real_memory) = registers[rt] & 0xff;
            break;
        case 2:
            *get_real_address_from(fake_address, real_memory) = registers[rt] & 0xff;
            *get_real_address_from(fake_address + 1, real_memory) = (registers[rt] & 0xff00) >> 8;
            break;
        case 1:
            *get_real_address_from(fake_address, real_memory) = registers[rt] & 0xff;
            *get_real_address_from(fake_address + 1, real_memory) = (registers[rt] & 0xff00) >> 8;
            *get_real_address_from(fake_address + 2, real_memory) = (registers[rt] & 0xff0000) >> 16;
            break;
        case 0:
            *get_real_address_from(fake_address, real_memory) = registers[rt] & 0xff;
            *get_real_address_from(fake_address + 1, real_memory) = (registers[rt] & 0xff00) >> 8;
            *get_real_address_from(fake_address + 2, real_memory) = (registers[rt] & 0xff0000) >> 16;
            *get_real_address_from(fake_address + 3, real_memory) = (registers[rt] & 0xff000000) >> 24;
            break;
    }
    return;
}

void _sc(uint32_t *&registers, uint32_t rs, uint32_t rt, int16_t offset, unsigned char *&real_memory) {
    uint32_t fake_address = (int32_t) registers[rs] + (int32_t) offset;
    if (fake_address & 3 != 0) {
        throw "not aligned";
    }
    *get_real_address_from(fake_address + 3, real_memory) = (uint8_t) ((registers[rt] & 0xff000000) >> 24);
    *get_real_address_from(fake_address + 2, real_memory) = (uint8_t) ((registers[rt] & 0xff0000) >> 16);
    *get_real_address_from(fake_address + 1, real_memory) = (uint8_t) ((registers[rt] & 0xff00) >> 8);
    *get_real_address_from(fake_address, real_memory) = (uint8_t) (registers[rt] & 0xff);
    return;
}

void _mfhi(uint32_t *&registers, uint32_t rd) {
    registers[rd] = registers[33];
    return;
}

void _mflo(uint32_t *&registers, uint32_t rd) {
    registers[rd] = registers[32];
    return;
}

void _mthi(uint32_t *&registers, uint32_t rs) {
    registers[33] = registers[rs];
    return;
}

void _mtlo(uint32_t *&registers, uint32_t rs) {
    registers[32] = registers[rs];
    return;
}

void _syscall(uint32_t *&registers, std::ifstream &input, std::ofstream &output, unsigned char * real_mem){
    switch (registers[2]) {
        case 1: {
            output << (int32_t) registers[4];
            output.flush();
            break;
        }
        case 4: {
            uint32_t cur = registers[4];
            while (true) {
                if (*get_real_address_from(cur, real_mem) == '\0') {
                    break;
                }
                output << *get_real_address_from(cur, real_mem);
                output.flush();
                cur += 1;
            }
            break;
        }
        case 5: {
            std::string res;
            std::getline(input, res);
            registers[2] = (uint32_t) (std::stoi(res, nullptr, 0));
            break;
        }
        case 8: {
            uint32_t num_bytes = registers[5];
            unsigned char *cur = get_real_address_from(registers[4], real_mem);
            if (num_bytes < 1) break;
            else if (num_bytes == 1) {
                *cur = 0x20;
                break;
            } else {
                std::string res;
                std::getline(input, res);
                for (int i = 0; i < num_bytes - 1; i++) {
                    *cur = res[i];
                    cur += 1;
                }
                *cur = 0x20;
                break;
            }
        }
        case 9: {
            uint32_t num_bytes = registers[4];
            registers[2] = registers[28];
            registers[28] = registers[28] + num_bytes;
            break;
        }
        case 10: {
            std::exit(EXIT_SUCCESS);
        }
        case 11: {
            output << (unsigned char) (registers[4] & 0xff);
            break;
        }
        case 12: {
            std::string res;
            std::getline(input, res);
            if (is_alldigit(res)) {
                registers[2] = std::stoi(res, nullptr, 0);
                break;
            } else {
                registers[2] = (unsigned char) res[0];
                break;
            }
        }
            //where is $a2 used?
        case 13: {
            unsigned char *cur = get_real_address_from(registers[4], real_mem);
            char* filename;
            while (true) {
                if (*cur == '\0') {
                    break;
                } else {
                    filename += *cur;
                }
            }
            registers[2] = open(filename,registers[5],registers[6]);
            break;
        }
        case 14: {
            registers[2] = read(registers[4],get_real_address_from(registers[5],real_mem),registers[6]);
            break;
        }
        case 15: {
            registers[2]=write(registers[4],get_real_address_from(registers[5],real_mem),registers[6]);
            break;
        }
        case 16: {
            close((int32_t) registers[4]);
            break;
        }
        case 17: {
            _exit((int32_t) registers[4]);
            break;
        };
    }
}

#endif //CSC3050PRO1_MIPS_OPR_H
