#include "MBC1.h"

void MBC1::writeByte(uint16_t addr, uint8_t val) {
    if(addr <= 0x1FFF) { //ram enable 
        ramEnabled = ((val & 0x0F) == 0x0A);
    }
    else if(addr <= 0x3FFF) { //rom bank number 
        uint8_t bank = val & 0x1F;
        if(bank == 0) bank = 1;
        romBank = (romBank & 0xE0) | bank;
    }
    else if(addr <= 0x5FFF) { //ram bank number / upper bits of rom bank number
        uint8_t bits = (val & 0x03);
        if(bankingMode) ramBank = bits;
        else {
            romBank = static_cast<uint8_t>((romBank & 0x1F) | (bits << 5));
        }
    }
    else if(addr <= 0x7FFF) {
        bankingMode = ((val & 0x01) != 0);
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF) { //external ram write
        if(ramEnabled && !eram.empty()) {
            uint32_t offset = static_cast<uint32_t>((bankingMode ? ramBank : 0) * 0x2000 + (addr - 0xA000));
            if(offset < eram.size()) {
                eram[offset] = val;
            }
        }
    }
}

uint8_t MBC1::readByte(uint16_t addr) {
    if(addr <= 0x3FFF) { //rom bank x0
        return rom[addr];
    }
    else if(addr <= 0x7FFF) { //rom bank (1-127)
        uint32_t offset = static_cast<uint32_t>(romBank*0x4000) + (addr - 0x4000);
        if(offset < rom.size()) return rom[offset];
        else return 0xFF;
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF) { //switchable external ram bank
        if (!ramEnabled || eram.empty()) return 0xFF; 
        uint32_t offset = static_cast<uint32_t>((bankingMode ? ramBank : 0) * 0x2000 + (addr - 0xA000));
        return (offset < eram.size()) ? eram[offset] : 0xFF;
    }
    return 0xFF;
}