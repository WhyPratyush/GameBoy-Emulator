#include "CPU.h"

void CPU::executeCB(uint8_t opcode) {
    switch (opcode) {
        
        

        default:
            printf("Unimplemented CB Opcode: 0x%02X at PC: 0x%04X\n", opcode, pc - 1);
            exit(1);
    }
}