#include "MBC5.h"

const std::vector<uint8_t> MBC5::getRam() const{
    return eram;
}

void MBC5::loadRam(const std::vector<uint8_t>& savedData) {
    size_t n = std::min(savedData.size(),eram.size());
    std::copy(savedData.begin(), savedData.begin() + static_cast<ptrdiff_t>(n), eram.begin());
}

void MBC5::writeByte(uint16_t addr, uint8_t val) {
    if(addr <= 0x1FFF) { //ram enable 
        ramEnabled = ((val & 0x0F) == 0x0A);
    }
    else if(addr <= 0x2FFF) { // low 8 bits of ROM bank
        romBank = (romBank & 0x100) | val;
    }
    else if(addr <= 0x3FFF) { //high bit of rom bank 
        romBank = (romBank & 0xFF) | (static_cast<uint16_t>(val & 0x01) << 8);
    }
    else if(addr <= 0x5FFF) { //ram bank number / upper bits of rom bank number
        if (hasRumble) {
            bool rumbleActive = (val & 0x08) != 0;
            ramBank = val & 0x07; 
            //TODO: rumble
        } 
        else ramBank = val & 0x0F;
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF) { //external ram write
        if(ramEnabled && !eram.empty()) {
            uint32_t offset = static_cast<uint32_t>(ramBank * 0x2000 + (addr - 0xA000));
            if(offset < eram.size()) {
                eram[offset] = val;
                isDirty = true;
            }
        }
    }
}

uint8_t MBC5::readByte(uint16_t addr) {
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
        uint32_t offset = static_cast<uint32_t>(ramBank * 0x2000 + (addr - 0xA000));
        return (offset < eram.size()) ? eram[offset] : 0xFF;
    }
    return 0xFF;
}
