# MIPS Assembler Simulator

##  If possible, merge the two struct (Instruction & _instruction) to deal with the redundancy. 

### Introduction

​		This report shows the method to run the codes, the general ideas to implement assembler and simulator, some details and tricks in implementation and the method I used for testing my code.

### 0. How to run it

​		The summited program, if unzipped, will be in this structure. All the codes are in the *src* file.

```
+-- report.pdf
+-- src
|  +-- _assembler.h
|  +-- _assembler.cpp
|  +-- MIPS_opr.h
|  +-- simulator.h
|  +-- simulator.cpp
|  +-- main.cpp
|  +-- CMakeLists.txt
```

To run it, (1) use *cmake* to compile the code; (2) run it:

```
cmake src
cmake --build .
./simulator mips.asm input.in output.out
```

Notice: The MIPS codes should be in the 1st file, the input data should be in the 2nd file and the output should be in the 3rd file.

### 1. Implementation

​		The program is consist of two parts: *assembler* and *simulator*. In *assembler*, the *text* and *data* strings are loaded from the MIPS codes, and the *text* is translated into binary forms according to the basic form of each type of instruction. In *simulator*, the program uses a specific memory space in size *6MB* to simulate the whole memory, and a specific memory space for 34 registers (32 normal ones, lo and hi) for *MIPS* code running. The data in registers and memory will be used in the operation.

#### 1.0 Assembler

​		There are mainly steps in this stage, scanning and processing the code, and then translating them into machine codes.

​		For convenience, a struct *Instruction* and three mapping is built. *Instruction* is a struct containing integer op, rt, rs, rd, shamt, funct, immediate, machine_code fields (initialized as 0), and string machine_code, jump-target field. One mapping is between *registers* and integers (0-31), like $a0 and 4. One other mapping is between string and *operation* (integer indeed), like "add" and Add. The last mapping, *symbol table* is between the label and its index in all instructions. The program firstly scans and processes the codes from file stream. Two flags, *text_flag* and *data_flag* are set for codes loading. When the *text_flag* is 1, the read in strings will be put in the vector *MIPS_text* and when the *data_flag* is 1, the strings will be put in the vector *MIPS_data* for static data. At the same time, all the comments, the substrings after # will be deleted. The label information is also recorded in the *symbol table*. 

​		After that, the program translates in instructions stored in the vector of instructions. The instruction strings are tokenized into different parts. The 1st part is used for distinguish which kind of instruction it is in a *switch* condition, where the current *Instruction* struct is built. In different case conditions, the data fields of the current *Instruction* are filled with the remained tokenized pieces. The jump targets and offsets are filled with the help of information in *symbol table.* Using a function, *ito_binary*, the integer fields can be translated into binary machine codes. All these instructions are stored in a vector of struct Instruction.

#### 1.1 Simulator

​		There are three steps in this stage, initializing memory space for simulation, pre-distinguishing and actual-performing.

​		The first part is space initialization. The program uses an *unsigned char array* to simulate the continuous 6MB simulated memory. *A uint32_t array with size 34* acts as the space for the 32 normal registers and the lo, hi registers. The *28th* registers, which stores the *global pointer*, is set to be 0x500000, to make the text segment have 1MB size. The *29th* registers, which stores the *stack pointer*, is set to be 0xA00000, which is at the end of the new memory. The pointer *real_memory* points to the new memory, serving as the *base address*. The pointer *heap_pointer*, is initialized as 0x500000. Then the static data and binary codes, which is stored in two vectors, are loaded into the simulated memory. After all the static data segment is put into the simulated memory, the heap_pointer is updated after the static data segment. 

​		The second part is pre-distinguish. For each binary code, an *_instruction* struct is built. The first 6 bits of binary code are loaded to the *_instruction* *op field*. In the following switch condition, the program uses the op code to distinguish the type (0 and 0x1c stand for R-type, 2 and 3 stand for J-type, the remained default ones stand for the I-type). After getting the type of the instruction, the corresponding date fields, (for example, *rs, rt, rd, shamt, funct* are useful for R-type instruction), are filled by the bits after masking the 32-bit binary code. After all the useful data fields are updated, the current *_instruction* struct, is used in the next part.

​		The third part is performing the codes. With the data in *_instruction* struct, the program can know what operations the current binary code stands for. For example, if the type is 0, the *funct* code is 0x20, and the *op* code is 0, the *switch* condition can direct it to the add function, where the *add* operation will be made.

### 2. Details and Tricks

#### 2.0 Memory space initialization

​		As for the 6MB simulated memory space, the program uses an unsigned char array to implement. As for the registers space, the program uses a uint32_t array to implement.

```c++
unsigned char * real_memory = new unsigned char[0x600000];
uint32_t * registers = new uint32_t[34];
```

​		In this initialization, all the values in these two arrays are initialized as 0. After that, registers[28] is set to be 0x500000, which is the value of global pointer, and registers[29] is set to be 0xA00000, which is the end of the stack and the initial position of the stack pointer.

​		The value of pointer real_memory serves as the base address in this simulation. The unsigned char array is much easier to be thought as the actual memory structure than the uint32_t integer array, since the smallest unit is 8 bits. This may make some operations like load word, which require the program to update 4 units in one operation, more difficult. But it makes the programmer easier to understand and makes some operations like load byte easier. Different from the simulation of real memory using unsigned char array, the registers space is initialized as uint32_t array for convenience, since most of operations take the 32 bits of registers as a whole unit. Using char array to represent registers may cause more difficulties.

#### 2.1 Memory related operations: endian matter

​		This program takes the *little endian*, which means for a word 0x1234, the byte 1 takes the highest address and the byte 4 takes the lowest address. Hence, when the program needs to make some store/load operations, it follows such pattern:

```
LSB: *get_real_address_from(fake_address+0)
...: *get_real_address_from(fake_address+1)
...: *get_real_address_from(fake_address+2)
MSB: *get_real_address_from(fake_address+3)
```

#### 2.2 Struct used in Assembler and Simulator

​		The struct to store instruction is used in this program for convenience. For assembler, the struct is like:

```c++
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
```

​		When loading the string code from the *MIPS* code file, the data fields mentioned in the current instruction will be updated, making the code easy to read and write. 

​		For simulator, the struct is similar, while the *target* field is an uint32_t value and there is no *machine_code* field.

```c++
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
```

With these data fields, the program can know which instruction it is and quickly call its corresponding implementation function.

```c++
switch (cur.type) {
            case 0:
                switch (cur.funct) {
                    case 0x20:
                        switch (cur.op) {
                            case 0x1c:
                                _clz(registers, cur.rs, cur.rd);
                                break;
......
```

#### 2.3 Get a certain byte / bit

​		Since the smallest unit of memory is 8-bit and the smallest unit of register is 32-bit, it is necessary to have some method to get a certain byte in an uint32_t value. The program uses shift and mask method:

```
LSB: (unsigned char)(a & 0xff)
...: (unsigned char)((a & 0xff00)>>8)
...: (unsigned char)((a & 0xff0000)>>16)
MSB: (unsigned char)((a & 0xff000000)>>24)
```

When dealing with the 32-bit binary code, similar method is used. For example, in *pre_distinguish* function, the program firstly picks the [31:26] bits for *op* code:

```
op = (uint32_t) ((ins & 0xfc000000) >> 26);
```

#### 2.4 Make switch "work" for strings

​		In assembler, the program needs to indicate the specific instruction the current MISP code string stands for. An intuitive solution is to use a 79-if statements, where the first piece of the tokenized instruction string serves as the condition:

```
if (instruction_pieces[0]=="add")..
...
else if (instruction_pieces[0]=="syscall")...
```

which takes θ(n) time to work.

​		*Switch* condition will work better than *if* condition. Hence the program makes a mapping between string and instruction to make switch works for string:

```
void opr_mapping(std::map<std::string, Operation> &opr) {
    opr["add"] = Add;
    opr["addu"] = Addu;
    opr["addi"] = Addi;
    .....
    }
switch(opr[instruction_pieces[0]]){
	case Add:
	....
}
```

### 3. Self-test

#### 3.0 Assembler test

​		To know whether the assembler works for the required instructions, I make a *test* function:

```c++
void test(std::string out, std::vector<Instruction *> &MIPS_binary) {
    std::fstream infile(out);
    std::string res;
    for (int i = 0; i < MIPS_binary.size(); i++) {
        std::getline(infile, res);
        if (res != MIPS_binary[i]->machine_code)
            std::cout << i << std::endl;
    }
}
```

which will compare my result binary with the correct binary code. If there are some differences, the programmer can trace back by the printed index.

#### 3.1 Simulator test

​		In addition to the given tests on Blackboard, I make some individual tests for some operations. For example, for the add operation implementation:

```c++
void _add(uint32_t *&registers, uint32_t rs, uint32_t rt, uint32_t rd, std::ofstream &out) {
    int32_t res = (int32_t) registers[rs] + (int32_t) registers[rt];
    if ((0x80000000 & registers[rs]) != (0x80000000 & res)) {
        if ((0x80000000 & registers[rs]) == (0x80000000 & registers[rt])) {
            out << "Integer_Overflow in Addition";
            std::exit(EXIT_FAILURE);
        }
    }
    registers[rd] = (uint32_t) res;
}
```

I will change the value of rs and rt and print the result, to check whether it can deal with some situations like overflow properly.

​		For operations like *lwl*, I make some MIPS code file to check whether they work.

```MIPS
.data
    .ascii "LKJI"  # memory
    .ascii "hgfe"  # register
    .ascii "hgfL"
    .ascii "hgLK"
    .ascii "hLKJ"
    .ascii "LKJI"
.text
    lui $t0, 80  # base address
    lw $t1, 4($t0)
    ......
    
```

If the output is correct (0000), I can know that it works properly.
