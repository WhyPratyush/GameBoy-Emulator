#include "CPU.h"

CPU::CPU(MMU* mmu_ptr) {
    this->mmu = mmu_ptr;
    reset();
}

void CPU::reset() {
    reg.reset();
    sp = 0xFFFE;
    pc = 0x0100;
}

uint8_t CPU::fetch() {
    uint8_t opcode = mmu->readByte(pc);
    if(halt_bug) halt_bug = false;
    else pc++;
    return opcode;
}

void CPU::push16(uint16_t val) {
    sp = static_cast<uint16_t>(sp - 2);
    mmu->writeWord(sp, val);
}

uint16_t CPU::pop16() {
    uint16_t val = mmu->readWord(sp);
    sp = static_cast<uint16_t>(sp + 2);
    return val;
}

uint8_t CPU::inc_r8(uint8_t val) {
    uint8_t res = val + 1;
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    reg.set_h_flag((val & 0x0F) == 0x0F);
    return res;
}

uint8_t CPU::dec_r8(uint8_t val) {
    uint8_t res = val - 1;
    reg.set_z_flag(res == 0);
    reg.set_n_flag(true);
    reg.set_h_flag((val & 0x0F) == 0x00);
    return res;
}

uint16_t CPU::add_hl_r16(uint16_t val) {
    uint16_t res = val + reg.get_hl();
    reg.set_n_flag(false);
    reg.set_h_flag(((reg.get_hl() & 0x0FFF) + (val & 0x0FFF)) > 0x0FFF);
    reg.set_c_flag((reg.get_hl()) > (0xFFFF - val));
    return res;
}

uint8_t CPU::add_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t result = val_a + val;
    reg.set_z_flag(result == 0);
    reg.set_n_flag(false);
    reg.set_h_flag((val_a & 0x0F) + (val & 0x0F) > 0x0F);
    reg.set_c_flag(val_a > (0xFF - val));
    return result;
}

uint8_t CPU::adc_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t carry = reg.get_c_flag() ? 1: 0;
    uint8_t result = static_cast<uint8_t>(val + val_a + carry);
    reg.set_z_flag(result == 0);
    reg.set_n_flag(false);
    reg.set_h_flag((val_a & 0x0F) + (val & 0x0F) + carry > 0x0F);
    reg.set_c_flag(val_a > (0xFF - val - carry));
    return result;
}

uint8_t CPU::sub_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t result = val_a - val;
    reg.set_z_flag(result == 0);
    reg.set_n_flag(true); 
    reg.set_h_flag((val_a & 0x0F) < (val & 0x0F)); 
    reg.set_c_flag(val_a < val); 
    return result;
}

uint8_t CPU::sbc_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t carry = reg.get_c_flag() ? 1 : 0;
    uint8_t result = static_cast<uint8_t>(val_a - val - carry);
    reg.set_z_flag(result == 0);
    reg.set_n_flag(true); 
    reg.set_h_flag((val_a & 0x0F) < ((val & 0x0F) + carry));
    reg.set_c_flag(val_a < val + carry);
    return result;
}

uint8_t CPU::and_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t res = val_a & val;
    reg.set_z_flag(res == 0);
    reg.set_c_flag(false);
    reg.set_n_flag(false);
    reg.set_h_flag(true);
    return res;
}

uint8_t CPU::xor_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t res = val_a ^ val;
    reg.set_z_flag(res == 0);
    reg.set_c_flag(false);
    reg.set_n_flag(false);
    reg.set_h_flag(false);
    return res;
}

uint8_t CPU::or_a_r8(uint8_t val) {
    uint8_t val_a = reg.get_a();
    uint8_t res = val_a | val;
    reg.set_z_flag(res == 0);
    reg.set_c_flag(false);
    reg.set_n_flag(false);
    reg.set_h_flag(false);
    return res;
}

uint16_t CPU::add_sp_imm8() {
    int8_t offset = static_cast<int8_t>(mmu->readByte(pc));
    pc++;
    reg.set_z_flag(false);
    reg.set_n_flag(false);
    reg.set_h_flag(((sp & 0x0F) + (offset & 0x0F)) > 0x0F);
    reg.set_c_flag(((sp & 0xFF) + (offset & 0xFF)) > 0xFF);
    return static_cast<uint16_t>(sp + offset);
}

uint8_t CPU::rlc(uint8_t val) {
    bool bit = (val & 0x80);
    uint8_t result = static_cast<uint8_t>((val << 1) | bit);
    reg.set_z_flag(result == 0);
    reg.set_n_flag(false);
    reg.set_h_flag(false);
    reg.set_c_flag(bit);
    return result;
}

uint8_t CPU::rrc(uint8_t val) {
    bool bit = (val & 0x01);
    uint8_t res = static_cast<uint8_t>((val >> 1) | (bit << 7));
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_c_flag(bit);
    reg.set_n_flag(false);
    return res;
}

uint8_t CPU::rl(uint8_t val) {
    bool bit = (val & 0x80);
    uint8_t res = static_cast<uint8_t>((val << 1) | reg.get_c_flag());
    reg.set_c_flag(bit);
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    return res;
}

uint8_t CPU::rr(uint8_t val) {
    bool bit = (val & 0x01);
    uint8_t res = static_cast<uint8_t>((val >> 1) | (reg.get_c_flag() << 7));
    reg.set_c_flag(bit);
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    return res;
}

uint8_t CPU::sla(uint8_t val) {
    bool bit = (val & 0x80);
    uint8_t res = static_cast<uint8_t>(val << 1);
    reg.set_c_flag(bit);
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    return res;
}

uint8_t CPU::sra(uint8_t val) {
    bool bit0 = (val & 0x01);
    bool bit7 = (val & 0x80);
    uint8_t res = static_cast<uint8_t>((val >> 1) | (bit7 << 7));
    reg.set_c_flag(bit0);
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    return res;
}

uint8_t CPU::srl(uint8_t val) {
    bool bit = (val & 0x01);
    uint8_t res = static_cast<uint8_t>(val >> 1);
    reg.set_c_flag(bit);
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    return res;
}

uint8_t CPU::swap(uint8_t val) {
    uint8_t low = static_cast<uint8_t>(val & 0x0F);
    uint8_t high = static_cast<uint8_t>(val & 0xF0);
    uint8_t res = static_cast<uint8_t>((high >> 4) | (low << 4));
    reg.set_c_flag(false);
    reg.set_h_flag(false);
    reg.set_z_flag(res == 0);
    reg.set_n_flag(false);
    return res;
}

void CPU::bit(uint8_t val, uint8_t b) {
    reg.set_z_flag(!((val >> b) & 1));
    reg.set_n_flag(false);
    reg.set_h_flag(true);
}

uint8_t CPU::get_r8(uint8_t index) {
    switch (index) {
        case 0: return reg.get_b();
        case 1: return reg.get_c();
        case 2: return reg.get_d();
        case 3: return reg.get_e();
        case 4: return reg.get_h();
        case 5: return reg.get_l();
        case 6: return mmu->readByte(reg.get_hl()); 
        default: return reg.get_a();
    }
}

void CPU::set_r8(uint8_t index, uint8_t val) {
    switch (index) {
        case 0: reg.set_b(val); break;
        case 1: reg.set_c(val); break;
        case 2: reg.set_d(val); break;
        case 3: reg.set_e(val); break;
        case 4: reg.set_h(val); break;
        case 5: reg.set_l(val); break;
        case 6: mmu->writeByte(reg.get_hl(), val); break;
        default: reg.set_a(val); break;
    }
}

uint8_t CPU::interrupt_handler() {
    if(scheduler > 0) {
        scheduler--;
        if(scheduler == 0) ime = true; 
    }

    uint8_t ie = mmu->readByte(0xFFFF);
    uint8_t iflag = mmu->readByte(0xFF0F);
    uint8_t interrupt = static_cast<uint8_t>(ie & iflag & 0x1F);

    if(interrupt != 0 && halted) halted = false;

    if((interrupt == 0 )|| !ime) return 0;
    ime = false;

    static const uint16_t addrs[5] = {0x0040,0x0048,0x0050,0x0058,0x0060};
    for(uint8_t bit = 0; bit < 5; bit++) {
        if(interrupt & (1 << bit)) {
            uint8_t current_if = mmu->readByte(0xFF0F);
            mmu->writeByte(0xFF0F, static_cast<uint8_t>(current_if & ~(1 << bit)));
            push16(pc);
            pc = addrs[bit];
            return 20;
        }
    }
    return 0;
}

bool CPU::is_halted() {
    return halted;
}
