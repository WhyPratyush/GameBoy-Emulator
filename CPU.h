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

        bool ime = false;
        bool halted = false;
        bool halt_bug = false;
        uint8_t scheduler = 0;
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
        uint8_t rlc(uint8_t val);
        uint8_t rrc(uint8_t val);
        uint8_t rl(uint8_t val);
        uint8_t rr(uint8_t val);
        uint8_t sla(uint8_t val);
        uint8_t sra(uint8_t val);
        uint8_t srl(uint8_t val);
        uint8_t swap(uint8_t val);
        void bit(uint8_t val, uint8_t b);
        uint8_t get_r8(uint8_t index);
        void set_r8(uint8_t index, uint8_t val);
        
    
    public:
        CPU(MMU* mmu_ptr);
        uint16_t get_pc() const { return pc; }
        bool get_ime() const { return ime; }
        void reset();
        uint8_t fetch();
        uint8_t interrupt_handler();
        uint8_t execute(uint8_t opcode);
        uint8_t executeCB(uint8_t opcode);
        bool is_halted();
};