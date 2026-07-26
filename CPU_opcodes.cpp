#include "CPU.h"

void CPU::execute(uint8_t opcode) {
    switch (opcode) {
        //BLOCK 0:
        //nop
        case 0x00:
            break;

        //ld r16,imm16
        case 0x01: //ld bc,imm16
            {
                uint16_t data = mmu->readWord(pc);
                pc += 2;
                reg.set_bc(data);
            }
            break;

        case 0x11: //ld de,imm16
            {
                uint16_t data = mmu->readWord(pc);
                pc += 2;
                reg.set_de(data);
            }
            break;
        
        case 0x21: //ld hl,imm16
            {
                uint16_t data = mmu->readWord(pc);
                pc += 2;
                reg.set_hl(data);
            }
            break;

        case 0x31: //ld sp,imm16
            {
                uint16_t data = mmu->readWord(pc);
                pc += 2;
                sp = data;
            }
            break;

        //ld [r16mem],a
        case 0x02: //ld [bc],a
            {
                mmu->writeByte(reg.get_bc(),reg.get_a());
            }
            break;

        case 0x12: //ld [de],a
            {
                mmu->writeByte(reg.get_de(),reg.get_a());
            }
            break;

        case 0x22: //ld [hl+],a (increase hl after setting it)
            {
                uint16_t hl = reg.get_hl();
                mmu->writeByte(hl,reg.get_a());
                reg.set_hl(static_cast<uint16_t>(hl+1));
            }
            break;

        case 0x32: //ld [hl-],a (decrease hl after setting it)
            {
                uint16_t hl = reg.get_hl();
                mmu->writeByte(hl,reg.get_a());
                reg.set_hl(static_cast<uint16_t>(hl-1));
            }
            break;

        //ld a,[r16mem]
        case 0x0A: //ld a,[bc]
            {
                reg.set_a(mmu->readByte(reg.get_bc()));
            }
            break;
            
        case 0x1A: //ld a,[de]
            {
                reg.set_a(mmu->readByte(reg.get_de()));
            }
            break;

        case 0x2A: //ld a,[hl+]
            {
                uint16_t hl = reg.get_hl();
                reg.set_a(mmu->readByte(hl));
                reg.set_hl(static_cast<uint16_t>(hl+1));
            }
            break;

        case 0x3A: //ld a,[hl-]
            {
                uint16_t hl = reg.get_hl();
                reg.set_a(mmu->readByte(hl));
                reg.set_hl(static_cast<uint16_t>(hl-1));
            }
            break;

        //ld [imm16],sp
        case 0x08:
            {
                uint16_t imm16 = mmu->readWord(pc);
                pc += 2;
                mmu->writeWord(imm16,sp);
            }
            break;
        
        //inc r16
        case 0x03: //inc bc
            {
                reg.set_bc(static_cast<uint16_t>(reg.get_bc()+1));
            }
            break;

        case 0x13: //inc de
            {
                reg.set_de(static_cast<uint16_t>(reg.get_de()+1));
            }
            break;

        case 0x23: //inc hl
            {
                reg.set_hl(static_cast<uint16_t>(reg.get_hl()+1));
            }
            break;

        case 0x33: //inc sp
            {
                sp++;
            }
            break;
        
        //dec r16
        case 0x0B: //dec bc
            {
                reg.set_bc(static_cast<uint16_t>(reg.get_bc()-1));
            }
            break;

        case 0x1B: //dec de
            {
                reg.set_de(static_cast<uint16_t>(reg.get_de()-1));
            }
            break;

        case 0x2B: //dec hl
            {
                reg.set_hl(static_cast<uint16_t>(reg.get_hl()-1));
            }
            break;

        case 0x3B: //dec sp
            {
                sp--;
            }
            break;
        
        //dec r8
        case 0x05: //dec b
            {
                reg.set_b(dec_r8(reg.get_b()));
            }
            break;

        case 0x0D: //dec c
            {
                reg.set_c(dec_r8(reg.get_c()));
            }
            break;
            
        case 0x15: //dec d
            {
                reg.set_d(dec_r8(reg.get_d()));
            }
            break;

        case 0x1D: //dec e
            {
                reg.set_e(dec_r8(reg.get_e()));
            }
            break;

        case 0x25: //dec h
            {
                reg.set_h(dec_r8(reg.get_h()));
            }
            break;

        case 0x2D: //dec l
            {
                reg.set_l(dec_r8(reg.get_l()));
            }
            break;

        case 0x35: //dec [hl]
            {
                uint16_t addr = reg.get_hl();
                uint8_t val = mmu->readByte(addr);
                mmu->writeByte(addr,dec_r8(val));
            }
            break;

        case 0x3D: //dec a
            {
                reg.set_a(dec_r8(reg.get_a()));
            }
            break;

        //inc r8
        case 0x04: //inc b
            {
                reg.set_b(inc_r8(reg.get_b()));
            }
            break;

        case 0x0C: //inc c
            {
                reg.set_c(inc_r8(reg.get_c()));
            }
            break;
            
        case 0x14: //inc d
            {
                reg.set_d(inc_r8(reg.get_d()));
            }
            break;

        case 0x1C: //inc e
            {
                reg.set_e(inc_r8(reg.get_e()));
            }
            break;

        case 0x24: //inc h
            {
                reg.set_h(inc_r8(reg.get_h()));
            }
            break;

        case 0x2C: //inc l
            {
                reg.set_l(inc_r8(reg.get_l()));
            }
            break;

        case 0x34: //inc [hl]
            {
                uint16_t addr = reg.get_hl();
                uint8_t val = mmu->readByte(addr);
                mmu->writeByte(addr,inc_r8(val));
            }
            break;

        case 0x3C: //inc a
            {
                reg.set_a(inc_r8(reg.get_a()));
            }
            break;

        //add hl, r16
        case 0x09: //add hl,bc
            {
                reg.set_hl(add_hl_r16(reg.get_bc()));
            }
            break;

        case 0x19: //add hl,de
            {
                reg.set_hl(add_hl_r16(reg.get_de()));
            }
            break;

        case 0x29: //add hl,hl
            {
                reg.set_hl(add_hl_r16(reg.get_hl()));
            }
            break;

        case 0x39: //add hl,sp
            {
                reg.set_hl(add_hl_r16(sp));
            }
            break;

        //ld r8,imm8
        case 0x06: //ld b
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_b(data);
            }
            break;

        case 0x0E: //ld c
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_c(data);
            }
            break;

        case 0x16: //ld d
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_d(data);
            }
            break;

        case 0x1E: //ld e
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_e(data);
            }
            break;

        case 0x26: //ld h
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_h(data);
            }
            break;

        case 0x2E: //ld l
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_l(data);
            }
            break;

        case 0x36: //ld [hl]
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                mmu->writeByte(reg.get_hl(),data);
            }
            break;

        case 0x3E: //ld a
            {
                uint8_t data = mmu->readByte(pc);
                pc++;
                reg.set_a(data);
            }
            break;

        case 0x07: //rlca - rotate a to left
            {
                uint8_t val = reg.get_a();
                bool bit = (val & 0x80);
                reg.set_a(static_cast<uint8_t>((val << 1) | bit));
                reg.set_h_flag(false);
                reg.set_z_flag(false);
                reg.set_c_flag(bit);
                reg.set_n_flag(false);
            }
            break;

        case 0x0F: //rrca - rotate a to right
            {
                uint8_t val = reg.get_a();
                bool bit = (val & 0x01);
                reg.set_a(static_cast<uint8_t>((val >> 1) | (bit << 7)));
                reg.set_h_flag(false);
                reg.set_z_flag(false);
                reg.set_c_flag(bit);
                reg.set_n_flag(false);
            }
            break;

        case 0x17: //rla - rotate a with c flag to left
            {
                uint8_t val = reg.get_a();
                bool bit = (val & 0x80);
                reg.set_a(static_cast<uint8_t>((val << 1) | reg.get_c_flag()));
                reg.set_c_flag(bit);
                reg.set_h_flag(false);
                reg.set_z_flag(false);
                reg.set_n_flag(false);
            }
            break;

        case 0x1F: //rra - rotate a with c flag to right
            {
                uint8_t val = reg.get_a();
                bool bit = (val & 0x01);
                reg.set_a(static_cast<uint8_t>((val >> 1) | (reg.get_c_flag() << 7)));
                reg.set_c_flag(bit);
                reg.set_h_flag(false);
                reg.set_z_flag(false);
                reg.set_n_flag(false);
            }
            break;

        case 0x27: //daa - decimal adjusted addition
            {
                uint8_t val = reg.get_a();
                uint8_t adjust = 0;
                bool carry = false;
                if(reg.get_n_flag()) {
                    if (reg.get_h_flag()) adjust -= 0x06;
                    if (reg.get_c_flag()) adjust -= 0x60;
                }
                else {
                    if (reg.get_h_flag() || (val & 0x0F) > 0x09) adjust += 0x06;
                    if (reg.get_c_flag() || val > 0x99) {
                        adjust += 0x60;
                        carry = true; 
                    }
                }

                val += adjust;
                reg.set_a(val);
                reg.set_z_flag(val == 0);
                reg.set_h_flag(false);
                if(carry) reg.set_c_flag(true);
            }
            break;

        case 0x2F: //cpl - complement a
            {
                uint8_t val = reg.get_a();
                reg.set_a(~val);
                reg.set_n_flag(true);
                reg.set_h_flag(true);
            }
            break;

        case 0x37: //scf - set carry flag
            {
                reg.set_c_flag(true);
                reg.set_n_flag(false);
                reg.set_h_flag(false);
            }
            break;

        case 0x3F: //ccf - complement carry flag
            {
                reg.set_c_flag(!reg.get_c_flag());
                reg.set_n_flag(false);
                reg.set_h_flag(false);
            }
            break;

        //jr - jump relative, basically takes an offset between -128 to 127 and pc += offset
        //only takes 2 bytes instead of 3 like jp
        case 0x18: //jr imm8
            {
                int8_t offset = static_cast<int8_t>(mmu->readByte(pc));
                pc++;
                pc = static_cast<uint16_t>(pc + offset);
            }
            break;

        case 0x20: //jr nz imm8 (jnz)
            {
                int8_t offset = static_cast<int8_t>(mmu->readByte(pc));
                pc++;
                if(!reg.get_z_flag()) {
                    pc = static_cast<uint16_t>(pc + offset);
                }
            }
            break;

        case 0x28: //jr z imm8 (jz)
            {
                int8_t offset = static_cast<int8_t>(mmu->readByte(pc));
                pc++;
                if(reg.get_z_flag()) {
                    pc = static_cast<uint16_t>(pc + offset);
                }
            }
            break;

        case 0x30: //jr nc imm8 (jnc)
            {
                int8_t offset = static_cast<int8_t>(mmu->readByte(pc));
                pc++;
                if(!reg.get_c_flag()) {
                    pc = static_cast<uint16_t>(pc + offset);
                }
            }
            break;

        case 0x38: //jr c imm8 (jc)
            {
                int8_t offset = static_cast<int8_t>(mmu->readByte(pc));
                pc++;
                if(reg.get_c_flag()) {
                    pc = static_cast<uint16_t>(pc + offset);
                }
            }
            break;

        case 0x10: //stop
            {
                pc++;
                //stops the cpu and puts lcd display into a low power mode until a button is pressed
                //idk how the fuck i'll do that but lets see
            }
            break;

        //BLOCK 1
        // ld r8,r8 
        case 0x40: //ld b,b
            {
                reg.set_b(reg.get_b());
            }
            break;

        case 0x41: //ld b,c
            {
                reg.set_b(reg.get_c());
            }
            break;

        case 0x42: //ld b,d
            {
                reg.set_b(reg.get_d());
            }
            break;
        
        case 0x43: //ld b,e
            {
                reg.set_b(reg.get_e());
            }
            break;

        case 0x44: //ld b,h
            {
                reg.set_b(reg.get_h());
            }
            break;

        case 0x45: //ld b,l
            {
                reg.set_b(reg.get_l());
            }
            break;

        case 0x46: //ld b,[hl]
            {
                reg.set_b(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x47: //ld b,a
            {
                reg.set_b(reg.get_a());
            }
            break;

        case 0x48: //ld c,b
            {
                reg.set_c(reg.get_b());
            }
            break;

        case 0x49: //ld c,c
            {
                reg.set_c(reg.get_c());
            }
            break;

        case 0x4A: //ld c,d
            {
                reg.set_c(reg.get_d());
            }
            break;
        
        case 0x4B: //ld c,e
            {
                reg.set_c(reg.get_e());
            }
            break;

        case 0x4C: //ld c,h
            {
                reg.set_c(reg.get_h());
            }
            break;

        case 0x4D: //ld c,l
            {
                reg.set_c(reg.get_l());
            }
            break;

        case 0x4E: //ld c,[hl]
            {
                reg.set_c(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x4F: //ld c,a
            {
                reg.set_c(reg.get_a());
            }
            break;

        case 0x50: //ld d,b
            {
                reg.set_d(reg.get_b());
            }
            break;

        case 0x51: //ld d,c
            {
                reg.set_d(reg.get_c());
            }
            break;

        case 0x52: //ld d,d
            {
                reg.set_d(reg.get_d());
            }
            break;
        
        case 0x53: //ld d,e
            {
                reg.set_d(reg.get_e());
            }
            break;

        case 0x54: //ld d,h
            {
                reg.set_d(reg.get_h());
            }
            break;

        case 0x55: //ld d,l
            {
                reg.set_d(reg.get_l());
            }
            break;

        case 0x56: //ld d,[hl]
            {
                reg.set_d(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x57: //ld d,a
            {
                reg.set_d(reg.get_a());
            }
            break;

        case 0x58: //ld e,b
            {
                reg.set_e(reg.get_b());
            }
            break;

        case 0x59: //ld e,c
            {
                reg.set_e(reg.get_c());
            }
            break;

        case 0x5A: //ld e,d
            {
                reg.set_e(reg.get_d());
            }
            break;
        
        case 0x5B: //ld e,e
            {
                reg.set_e(reg.get_e());
            }
            break;

        case 0x5C: //ld e,h
            {
                reg.set_e(reg.get_h());
            }
            break;

        case 0x5D: //ld e,l
            {
                reg.set_e(reg.get_l());
            }
            break;

        case 0x5E: //ld e,[hl]
            {
                reg.set_e(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x5F: //ld e,a
            {
                reg.set_e(reg.get_a());
            }
            break;
        
        case 0x60: //ld h,b
            {
                reg.set_h(reg.get_b());
            }
            break;

        case 0x61: //ld h,c
            {
                reg.set_h(reg.get_c());
            }
            break;

        case 0x62: //ld h,d
            {
                reg.set_h(reg.get_d());
            }
            break;
        
        case 0x63: //ld h,e
            {
                reg.set_h(reg.get_e());
            }
            break;

        case 0x64: //ld h,h
            {
                reg.set_h(reg.get_h());
            }
            break;

        case 0x65: //ld h,l
            {
                reg.set_h(reg.get_l());
            }
            break;

        case 0x66: //ld h,[hl]
            {
                reg.set_h(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x67: //ld h,a
            {
                reg.set_h(reg.get_a());
            }
            break;

        case 0x68: //ld l,b
            {
                reg.set_l(reg.get_b());
            }
            break;

        case 0x69: //ld l,c
            {
                reg.set_l(reg.get_c());
            }
            break;

        case 0x6A: //ld l,d
            {
                reg.set_l(reg.get_d());
            }
            break;
        
        case 0x6B: //ld l,e
            {
                reg.set_l(reg.get_e());
            }
            break;

        case 0x6C: //ld l,h
            {
                reg.set_l(reg.get_h());
            }
            break;

        case 0x6D: //ld l,l
            {
                reg.set_l(reg.get_l());
            }
            break;

        case 0x6E: //ld l,[hl]
            {
                reg.set_l(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x6F: //ld l,a
            {
                reg.set_l(reg.get_a());
            }
            break;

        case 0x70: //ld [hl],b;
            {
                mmu->writeByte(reg.get_hl(),reg.get_b());
            }
            break;

        case 0x71: //ld [hl],c;
            {
                mmu->writeByte(reg.get_hl(),reg.get_c());
            }
            break;

        case 0x72: //ld [hl],d;
            {
                mmu->writeByte(reg.get_hl(),reg.get_d());
            }
            break;

        case 0x73: //ld [hl],e;
            {
                mmu->writeByte(reg.get_hl(),reg.get_e());
            }
            break;

        case 0x74: //ld [hl],h;
            {
                mmu->writeByte(reg.get_hl(),reg.get_h());
            }
            break;

        case 0x75: //ld [hl],l;
            {
                mmu->writeByte(reg.get_hl(),reg.get_l());
            }
            break;

        case 0x76: //ld [hl],[hl]; - halt
            {
                
            }
            break;

        case 0x77: //ld [hl],a;
            {
                mmu->writeByte(reg.get_hl(),reg.get_a());
            }
            break;

        case 0x78: //ld a,b
            {
                reg.set_a(reg.get_b());
            }
            break;

        case 0x79: //ld a,c
            {
                reg.set_a(reg.get_c());
            }
            break;

        case 0x7A: //ld a,d
            {
                reg.set_a(reg.get_d());
            }
            break;
        
        case 0x7B: //ld a,e
            {
                reg.set_a(reg.get_e());
            }
            break;

        case 0x7C: //ld a,h
            {
                reg.set_a(reg.get_h());
            }
            break;

        case 0x7D: //ld a,l
            {
                reg.set_a(reg.get_l());
            }
            break;

        case 0x7E: //ld a,[hl]
            {
                reg.set_a(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0x7F: //ld a,a
            {
                reg.set_a(reg.get_a());
            }
            break;

        //BLOCK 2
        //add a,r8
        case 0x80: //add a,b
            {
                reg.set_a(add_a_r8(reg.get_b()));
            }
            break;

        case 0x81: //add a,c
            {
                reg.set_a(add_a_r8(reg.get_c()));
            }
            break;

        case 0x82: //add a,d
            {
                reg.set_a(add_a_r8(reg.get_d()));
            }
            break;

        case 0x83: //add a,e
            {
                reg.set_a(add_a_r8(reg.get_e()));
            }
            break;

        case 0x84: //add a,h
            {
                reg.set_a(add_a_r8(reg.get_h()));
            }
            break;

        case 0x85: //add a,l
            {
                reg.set_a(add_a_r8(reg.get_l()));
            }
            break;

        case 0x86: //add a,[hl]
            {
                reg.set_a(add_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0x87: //add a,a
            {
                reg.set_a(add_a_r8(reg.get_a()));
            }
            break;

        //adc a,r8
        case 0x88: //adc a,b
            {
                reg.set_a(adc_a_r8(reg.get_b()));
            }
            break;

        case 0x89: //adc a,c
            {
                reg.set_a(adc_a_r8(reg.get_c()));
            }
            break;

        case 0x8A: //adc a,d
            {
                reg.set_a(adc_a_r8(reg.get_d()));
            }
            break;

        case 0x8B: //adc a,e
            {
                reg.set_a(adc_a_r8(reg.get_e()));
            }
            break;

        case 0x8C: //adc a,h
            {
                reg.set_a(adc_a_r8(reg.get_h()));
            }
            break;

        case 0x8D: //adc a,l
            {
                reg.set_a(adc_a_r8(reg.get_l()));
            }
            break;

        case 0x8E: //adc a,[hl]
            {
                reg.set_a(adc_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0x8F: //adc a,a
            {
                reg.set_a(adc_a_r8(reg.get_a()));
            }
            break;
            
        //sub a, r8
        case 0x90: //sub a,b
            {
                reg.set_a(sub_a_r8(reg.get_b()));
            }
            break;

        case 0x91: //sub a,c
            {
                reg.set_a(sub_a_r8(reg.get_c()));
            }
            break;

        case 0x92: //sub a,d
            {
                reg.set_a(sub_a_r8(reg.get_d()));
            }
            break;

        case 0x93: //sub a,e
            {
                reg.set_a(sub_a_r8(reg.get_e()));
            }
            break;

        case 0x94: //sub a,h
            {
                reg.set_a(sub_a_r8(reg.get_h()));
            }
            break;

        case 0x95: //sub a,l
            {
                reg.set_a(sub_a_r8(reg.get_l()));
            }
            break;

        case 0x96: //sub a,[hl]
            {
                reg.set_a(sub_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0x97: //sub a,a
            {
                reg.set_a(sub_a_r8(reg.get_a()));
            }
            break;

        //sbc a, r8
        case 0x98: //sbc a,b
            {
                reg.set_a(sbc_a_r8(reg.get_b()));
            }
            break;

        case 0x99: //sbc a,c
            {
                reg.set_a(sbc_a_r8(reg.get_c()));
            }
            break;

        case 0x9A: //sbc a,d
            {
                reg.set_a(sbc_a_r8(reg.get_d()));
            }
            break;

        case 0x9B: //sbc a,e
            {
                reg.set_a(sbc_a_r8(reg.get_e()));
            }
            break;

        case 0x9C: //sbc a,h
            {
                reg.set_a(sbc_a_r8(reg.get_h()));
            }
            break;

        case 0x9D: //sbc a,l
            {
                reg.set_a(sbc_a_r8(reg.get_l()));
            }
            break;

        case 0x9E: //sbc a,[hl]
            {
                reg.set_a(sbc_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0x9F: //sbc a,a
            {
                reg.set_a(sbc_a_r8(reg.get_a()));
            }
            break;

        //and a,r8
        case 0xA0: //and a,b
            {
                reg.set_a(and_a_r8(reg.get_b()));
            }
            break;

        case 0xA1: //and a,c
            {
                reg.set_a(and_a_r8(reg.get_c()));
            }
            break;

        case 0xA2: //and a,d
            {
                reg.set_a(and_a_r8(reg.get_d()));
            }
            break;

        case 0xA3: //and a,e
            {
                reg.set_a(and_a_r8(reg.get_e()));
            }
            break;
        
        case 0xA4: //and a,h
            {
                reg.set_a(and_a_r8(reg.get_h()));
            }
            break;

        case 0xA5: //and a,l
            {
                reg.set_a(and_a_r8(reg.get_l()));
            }
            break;

        case 0xA6: //and a,[hl]
            {
                reg.set_a(and_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0xA7: //and a,a
            {
                reg.set_a(and_a_r8(reg.get_a()));
            }
            break;

        //xor a,r8
        case 0xA8: //xor a,b
            {
                reg.set_a(xor_a_r8(reg.get_b()));
            }
            break;

        case 0xA9: //xor a,c
            {
                reg.set_a(xor_a_r8(reg.get_c()));
            }
            break;

        case 0xAA: //xor a,d
            {
                reg.set_a(xor_a_r8(reg.get_d()));
            }
            break;

        case 0xAB: //xor a,e
            {
                reg.set_a(xor_a_r8(reg.get_e()));
            }
            break;

        case 0xAC: //xor a,h
            {
                reg.set_a(xor_a_r8(reg.get_h()));
            }
            break;

        case 0xAD: //xor a,l
            {
                reg.set_a(xor_a_r8(reg.get_l()));
            }
            break;

        case 0xAE: //xor a,[hl]
            {
                reg.set_a(xor_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0xAF: //xor a,a
            {
                reg.set_a(xor_a_r8(reg.get_a()));
            }
            break;

        //or a,r8
        case 0xB0: //or a,b
            {
                reg.set_a(or_a_r8(reg.get_b()));
            }
            break;

        case 0xB1: //or a,c
            {
                reg.set_a(or_a_r8(reg.get_c()));
            }
            break;

        case 0xB2: //or a,d
            {
                reg.set_a(or_a_r8(reg.get_d()));
            }
            break;

        case 0xB3: //or a,e
            {
                reg.set_a(or_a_r8(reg.get_e()));
            }
            break;

        case 0xB4: //or a,h
            {
                reg.set_a(or_a_r8(reg.get_h()));
            }
            break;

        case 0xB5: //or a,l
            {
                reg.set_a(or_a_r8(reg.get_l()));
            }
            break;

        case 0xB6: //or a,[hl]
            {
                reg.set_a(or_a_r8(mmu->readByte(reg.get_hl())));
            }
            break;

        case 0xB7: //or a,a
            {
                reg.set_a(or_a_r8(reg.get_a()));
            }
            break;

        //cp a,r8 - compare a with r8
        case 0xB8: //cp a,b
            {
                sub_a_r8(reg.get_b());
            }
            break;

        case 0xB9: //cp a,c
            {
                sub_a_r8(reg.get_c());
            }
            break;

        case 0xBA: //cp a,d
            {
                sub_a_r8(reg.get_d());
            }
            break;

        case 0xBB: //cp a,e
            {
                sub_a_r8(reg.get_e());
            }
            break;

        case 0xBC: //cp a,h
            {
                sub_a_r8(reg.get_h());
            }
            break;

        case 0xBD: //cp a,l
            {
                sub_a_r8(reg.get_l());
            }
            break;

        case 0xBE: //cp a,[hl]
            {
                sub_a_r8(mmu->readByte(reg.get_hl()));
            }
            break;

        case 0xBF: //cp a,a
            {
                sub_a_r8(reg.get_a());
            }
            break;
            
        case 0xC6: //add a,imm8
            {
                reg.set_a(add_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xCE: //adc a,imm8
            {
                reg.set_a(adc_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xD6: //sub a,imm8
            {
                reg.set_a(sub_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xDE: //sbc a,imm8
            {
                reg.set_a(sbc_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xE6: //and a,imm8
            {
                reg.set_a(and_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xEE: //xor a,imm8
            {
                reg.set_a(xor_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xF6: //or a,imm8
            {
                reg.set_a(or_a_r8(mmu->readByte(pc)));
                pc++;
            }
            break;

        case 0xFE: //cp a,imm8
            {
                sub_a_r8(mmu->readByte(pc));
                pc++;
            }
            break;

        case 0xE9: //jp hl
            {
                pc = reg.get_hl();
            }
            break;

        case 0xC3: //jp imm16
            {
                uint16_t val = mmu->readWord(pc);
                pc = val;
            }
            break;

        //jp cond, imm16
        case 0xC2: //jp nz, imm16
            {
                uint16_t val = mmu->readWord(pc);
                pc += 2;
                if(!reg.get_z_flag()) pc = val;
            }
            break;

        case 0xCA: //jp z,imm16
            {
                uint16_t val = mmu->readWord(pc);
                pc += 2;
                if(reg.get_z_flag()) pc = val;
            }
            break;

        case 0xD2: //jp nc,imm16
            {
                uint16_t val = mmu->readWord(pc);
                pc += 2;
                if(!reg.get_c_flag()) pc = val;
            }
            break;

        case 0xDA: //jp c, imm16
            {
                uint16_t val = mmu->readWord(pc);
                pc += 2;
                if(reg.get_c_flag()) pc = val;
            }
            break;

        //pop r16stk
        case 0xC1: //pop bc
            {
                reg.set_bc(pop16());
            }
            break;

        case 0xD1: //pop de
            {
                reg.set_de(pop16());
            }
            break;

        case 0xE1: //pop hl
            {
                reg.set_hl(pop16());
            }
            break;

        case 0xF1: //pop af
            {
                reg.set_af(pop16());
            }
            break;

        //push r16stk
        case 0xC5: //push bc
            {
                push16(reg.get_bc());
            }
            break;

        case 0xD5: //push de
            {
                push16(reg.get_de());
            }
            break;

        case 0xE5: //push hl
            {
                push16(reg.get_hl());
            }
            break;

        case 0xF5: //push af
            {
                push16(reg.get_af());
            }
            break;

        case 0xC9: //ret
            {
                pc = pop16();
            }
            break;

        case 0xD9: //reti
            {
                pc = pop16();
                // TODO: ime = true;
            }
            break;

        case 0xC0: //ret nz
            {
                if(!reg.get_z_flag()) pc = pop16();
            }
            break;

        case 0xC8:
            {
                if(reg.get_z_flag()) pc = pop16();
            }
            break;

        case 0xD0: //ret nc
            {
                if(!reg.get_c_flag()) pc = pop16();
            }
            break;

        case 0xD8: //ret c
            {
                if(reg.get_c_flag()) pc = pop16();
            }
            break;

        case 0xCD: //call imm16
            {
                uint16_t target = mmu->readWord(pc);
                pc = static_cast<uint16_t>(pc + 2);
                push16(pc);      
                pc = target;
            }
            break;

        case 0xC4: //call nz,imm16
            {
                uint16_t target = mmu->readWord(pc);
                pc = static_cast<uint16_t>(pc + 2);
                if (!reg.get_z_flag()) {
                    push16(pc);
                    pc = target;
                }
            }
            break;

        case 0xCC: //call z,imm16
            {
                uint16_t target = mmu->readWord(pc);
                pc = static_cast<uint16_t>(pc + 2);
                if (reg.get_z_flag()) {
                    push16(pc);
                    pc = target;
                }
            }
            break;

        case 0xD4: //call nc,imm16
            {
                uint16_t target = mmu->readWord(pc);
                pc = static_cast<uint16_t>(pc + 2);
                if (!reg.get_c_flag()) {
                    push16(pc);
                    pc = target;
                }
            }
            break;

        case 0xDC: //call c,imm16
            {
                uint16_t target = mmu->readWord(pc);
                pc = static_cast<uint16_t>(pc + 2);
                if (reg.get_c_flag()) {
                    push16(pc);
                    pc = target;
                }
            }
            break;

        //rst tgt3 
        case 0xC7: 
        case 0xCF: 
        case 0xD7: 
        case 0xDF: 
        case 0xE7: 
        case 0xEF: 
        case 0xF7: 
        case 0xFF: 
            {
                push16(pc);
                uint16_t target = opcode & 0x38;
                pc = target;
            }
            break;

        //ldh- high ram loads
        case 0xE0: // LDH [imm8], A
            {
                uint8_t offset = mmu->readByte(pc);
                pc++;
                mmu->writeByte(0xFF00 + offset, reg.get_a());
            }
            break;

        case 0xF0: // LDH A, [imm8]
            {
                uint8_t offset = mmu->readByte(pc);
                pc++;
                reg.set_a(mmu->readByte(0xFF00 + offset));
            }
            break;

        case 0xE2: // LDH [C], A
            {
                mmu->writeByte(0xFF00 + reg.get_c(), reg.get_a());
            }
            break;

        case 0xF2: // LDH A, [C]
            {
                reg.set_a(mmu->readByte(0xFF00 + reg.get_c()));
            }
            break;

        case 0xEA: // LD [imm16], A
            {
                uint16_t addr = mmu->readWord(pc);
                pc += 2;
                mmu->writeByte(addr, reg.get_a());
            }
            break;

        case 0xFA: // LD A, [imm16]
            {
                uint16_t addr = mmu->readWord(pc);
                pc += 2;
                reg.set_a(mmu->readByte(addr));
            }
            break;

            case 0xF9: // LD SP, HL
            {
                sp = reg.get_hl();
            }
            break;

        case 0xE8: // ADD SP, imm8
            {
                sp = add_sp_imm8();
            }
            break;

        case 0xF8: // LD HL, SP + imm8
            {
                reg.set_hl(add_sp_imm8());
            }
            break;

        case 0xF3: // DI (Disable Interrupts)
            {
                // TODO: ime = false;
            }
            break;

        case 0xFB: // EI (Enable Interrupts)
            {
                // TODO: ime = true;
            }
            break;

        case 0xCB://prefix cb
            {
                uint8_t cb_opcode = mmu->readByte(pc);
                pc++;
                executeCB(cb_opcode);
            }
            break;

        default:
            printf("Unimplemented Opcode: 0x%02X at PC: 0x%04X\n", opcode, pc - 1);
            exit(1);
        
    }
}