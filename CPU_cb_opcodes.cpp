#include "CPU.h"

void CPU::executeCB(uint8_t opcode) {
    switch (opcode) {
        //rlc r8
        case 0x00: //rlc b
            {
                reg.set_b(rlc(reg.get_b()));
            }
            break;

        case 0x01: //rlc c
            {
                reg.set_c(rlc(reg.get_c()));
            }
            break;

        case 0x02: //rlc d
            {
                reg.set_d(rlc(reg.get_d()));
            }
            break;

        case 0x03: //rlc e
            {
                reg.set_e(rlc(reg.get_e()));
            }
            break;

        case 0x04: //rlc h
            {
                reg.set_h(rlc(reg.get_h()));
            }
            break;

        case 0x05: //rlc l
            {
                reg.set_l(rlc(reg.get_l()));
            }
            break;

        case 0x06: //rlc [hl]
            {
                uint8_t val = rlc(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x07: //rlc a
            {
                reg.set_a(rlc(reg.get_a()));
            }
            break;
        
        //rrc r8
        case 0x08: //rrc b
            {
                reg.set_b(rrc(reg.get_b()));
            }
            break;

        case 0x09: //rrc c
            {
                reg.set_c(rrc(reg.get_c()));
            }
            break;

        case 0x0A: //rrc d
            {
                reg.set_d(rrc(reg.get_d()));
            }
            break;

        case 0x0B: //rrc e
            {
                reg.set_e(rrc(reg.get_e()));
            }
            break;

        case 0x0C: //rrc h
            {
                reg.set_h(rrc(reg.get_h()));
            }
            break;

        case 0x0D: //rrc l
            {
                reg.set_l(rrc(reg.get_l()));
            }
            break;

        case 0x0E: //rrc [hl]
            {
                uint8_t val = rrc(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x0F: //rrc a
            {
                reg.set_a(rrc(reg.get_a()));
            }
            break;

        //rl r8
        case 0x10: //rl b
            {
                reg.set_b(rl(reg.get_b()));
            }
            break;

        case 0x11: //rl c
            {
                reg.set_c(rl(reg.get_c()));
            }
            break;

        case 0x12: //rl d
            {
                reg.set_d(rl(reg.get_d()));
            }
            break;

        case 0x13: //rl e
            {
                reg.set_e(rl(reg.get_e()));
            }
            break;

        case 0x14: //rl h
            {
                reg.set_h(rl(reg.get_h()));
            }
            break;

        case 0x15: //rl l
            {
                reg.set_l(rl(reg.get_l()));
            }
            break;

        case 0x16: //rl [hl]
            {
                uint8_t val = rl(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x17: //rl a
            {
                reg.set_a(rl(reg.get_a()));
            }
            break;

        //rr r8
        case 0x18: //rr b
            {
                reg.set_b(rr(reg.get_b()));
            }
            break;

        case 0x19: //rr c
            {
                reg.set_c(rr(reg.get_c()));
            }
            break;

        case 0x1A: //rr d
            {
                reg.set_d(rr(reg.get_d()));
            }
            break;

        case 0x1B: //rr e
            {
                reg.set_e(rr(reg.get_e()));
            }
            break;

        case 0x1C: //rr h
            {
                reg.set_h(rr(reg.get_h()));
            }
            break;

        case 0x1D: //rr l
            {
                reg.set_l(rr(reg.get_l()));
            }
            break;

        case 0x1E: //rr [hl]
            {
                uint8_t val = rr(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x1F: //rr a
            {
                reg.set_a(rr(reg.get_a()));
            }
            break;

        //sla r8
        case 0x20: //sla b
            {
                reg.set_b(sla(reg.get_b()));
            }
            break;

        case 0x21: //sla c
            {
                reg.set_c(sla(reg.get_c()));
            }
            break;

        case 0x22: //sla d
            {
                reg.set_d(sla(reg.get_d()));
            }
            break;

        case 0x23: //sla e
            {
                reg.set_e(sla(reg.get_e()));
            }
            break;

        case 0x24: //sla h
            {
                reg.set_h(sla(reg.get_h()));
            }
            break;

        case 0x25: //sla l
            {
                reg.set_l(sla(reg.get_l()));
            }
            break;

        case 0x26: //sla [hl]
            {
                uint8_t val = sla(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x27: //sla a
            {
                reg.set_a(sla(reg.get_a()));
            }
            break;

        //sra r8
        case 0x28: //sra b
            {
                reg.set_b(sra(reg.get_b()));
            }
            break;

        case 0x29: //sra c
            {
                reg.set_c(sra(reg.get_c()));
            }
            break;

        case 0x2A: //sra d
            {
                reg.set_d(sra(reg.get_d()));
            }
            break;

        case 0x2B: //sra e
            {
                reg.set_e(sra(reg.get_e()));
            }
            break;

        case 0x2C: //sra h
            {
                reg.set_h(sra(reg.get_h()));
            }
            break;

        case 0x2D: //sra l
            {
                reg.set_l(sra(reg.get_l()));
            }
            break;

        case 0x2E: //sra [hl]
            {
                uint8_t val = sra(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x2F: //sra a
            {
                reg.set_a(sra(reg.get_a()));
            }
            break;

        //swap r8
        case 0x30: //swap b
            {
                reg.set_b(swap(reg.get_b()));
            }
            break;

        case 0x31: //swap c
            {
                reg.set_c(swap(reg.get_c()));
            }
            break;

        case 0x32: //swap d
            {
                reg.set_d(swap(reg.get_d()));
            }
            break;

        case 0x33: //swap e
            {
                reg.set_e(swap(reg.get_e()));
            }
            break;

        case 0x34: //swap h
            {
                reg.set_h(swap(reg.get_h()));
            }
            break;

        case 0x35: //swap l
            {
                reg.set_l(swap(reg.get_l()));
            }
            break;

        case 0x36: //swap [hl]
            {
                uint8_t val = swap(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x37: //swap a
            {
                reg.set_a(swap(reg.get_a()));
            }
            break;

        //srl r8
        case 0x38: //srl b
            {
                reg.set_b(srl(reg.get_b()));
            }
            break;

        case 0x39: //srl c
            {
                reg.set_c(srl(reg.get_c()));
            }
            break;

        case 0x3A: //srl d
            {
                reg.set_d(srl(reg.get_d()));
            }
            break;

        case 0x3B: //srl e
            {
                reg.set_e(srl(reg.get_e()));
            }
            break;

        case 0x3C: //srl h
            {
                reg.set_h(srl(reg.get_h()));
            }
            break;

        case 0x3D: //srl l
            {
                reg.set_l(srl(reg.get_l()));
            }
            break;

        case 0x3E: //srl [hl]
            {
                uint8_t val = srl(mmu->readByte(reg.get_hl()));
                mmu->writeByte(reg.get_hl(),val);
            }
            break;

        case 0x3F: //srl a
            {
                reg.set_a(srl(reg.get_a()));
            }
            break;

        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F:
            {
                uint8_t bit_index = (opcode >> 3) & 0x07;
                uint8_t reg_index = opcode & 0x07;
                bit(get_r8(reg_index), bit_index);
            }
            break;

        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF:
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
            {
                uint8_t bit_index = (opcode >> 3) & 0x07;
                uint8_t reg_index = opcode & 0x07;
                uint8_t val = get_r8(reg_index);
                set_r8(reg_index, static_cast<uint8_t>(val & ~(1 << bit_index)));
            }
            break;

        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7:
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
            {
                uint8_t bit_index = (opcode >> 3) & 0x07;
                uint8_t reg_index = opcode & 0x07;
                uint8_t val = get_r8(reg_index);
                set_r8(reg_index, static_cast<uint8_t>(val | (1 << bit_index)));
            }
            break;

        default:
            printf("Unimplemented CB Opcode: 0x%02X at PC: 0x%04X\n", opcode, pc - 1);
            exit(1);
    }
}