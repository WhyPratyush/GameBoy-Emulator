#pragma once

#include <cstdint>
#include "MMU.h"
#include <cstdio>
#include <cstdlib>
#include "Registers.h"

class CPU {
    private:
        Registers reg;
        MMU* mmu;

        uint16_t sp, pc;
        void push16(uint16_t val);
        uint16_t pop16();
        
        uint8_t inc_r8(uint8_t val);
        uint8_t dec_r8(uint8_t val);
        uint16_t add_hl_r16(uint16_t val);
        
        uint8_t add_a_r8(uint8_t val);
        uint8_t adc_a_r8(uint8_t val);
        uint8_t sub_a_r8(uint8_t val);
        uint8_t sbc_a_r8(uint8_t val);
        uint8_t and_a_r8(uint8_t val);
        uint8_t xor_a_r8(uint8_t val);
        uint8_t or_a_r8(uint8_t val);
        uint16_t add_sp_imm8();
    
    public:
        CPU(MMU* mmu_ptr);
        void reset();
        uint8_t fetch();
        
        void execute(uint8_t opcode);
        void executeCB(uint8_t opcode);
};