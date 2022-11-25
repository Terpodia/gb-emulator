#include <instructions.h>

instruction instructions[0x100];

void setup_instructions_types(){
    for(int i=0; i<0x100; i++) instructions[i] = {IN_NONE};

    instructions[0x00] = {IN_NOP, AM_IMP},
    instructions[0x05] = {IN_DEC, AM_R, RT_B},
    instructions[0x0E] = {IN_LD, AM_R_D8, RT_C},
    instructions[0xAF] = {IN_XOR, AM_R, RT_A},
    instructions[0xC3] = {IN_JP, AM_D16};
}

instruction* instruction_by_opcode(BYTE opcode){
    return instructions + opcode;
}