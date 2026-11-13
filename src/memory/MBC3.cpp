#include "MBC3.h"
#include <fstream>

void MBC3::writeByte(uint16_t addr, uint8_t val) {
    if(addr <= 0x1FFF) { //ram and rtc enable
        ramRtcEnabled = ((val & 0x0F) == 0x0A);
    }
    else if(addr <= 0x3FFF) { //rom bank no.
        romBank = val & romMask; 
        if (romBank == 0) romBank = 1;
    }
    else if(addr <= 0x5FFF) { //ram bank no./rtc reg select
        ramRtcSelect = val & 0x0F;
    }
    else if(addr <= 0x7FFF) { //latch clock data
        rtc.writeLatch(val);
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF) { //eram / rtc write
        if(ramRtcEnabled) {
            if(ramRtcSelect <= 0x07 && !eram.empty()) {
                uint32_t offset = static_cast<uint32_t>((ramRtcSelect * 0x2000) + (addr-0xA000));
                if(offset < eram.size()) {
                    eram[offset] = val;
                    isDirty = true;
                }
            }
            else if(ramRtcSelect >= 0x08 && ramRtcSelect <= 0x0C) {
                rtc.writeReg(ramRtcSelect,val);
                isDirty = true;
            }
        }
    }
}

uint8_t MBC3::readByte(uint16_t addr) {
    if(addr <= 0x3FFF) return rom[addr]; //rom bank 0
    else if(addr <= 0x7FFF) { //switchable rom bank
        uint32_t offset = static_cast<uint32_t>((romBank * 0x4000) + (addr -0x4000));
        return (offset < rom.size()) ? rom[offset] : 0xFF;
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF) { //eram / rtc read
        if(!ramRtcEnabled) return 0xFF;
        if(ramRtcSelect <= 0x07 && !eram.empty()) { //eram Read
            uint32_t offset = static_cast<uint32_t>((ramRtcSelect * 0x2000) + (addr - 0xA000));
            return (offset < eram.size()) ? eram[offset] : 0xFF;
        } 
        else if(ramRtcSelect >= 0x08 && ramRtcSelect <= 0x0C) {
            return rtc.readReg(ramRtcSelect);
        }
    }
    return 0xFF;
}

const std::vector<uint8_t> MBC3::getRam() const{
    std::vector<uint8_t> data = eram;
    std::vector<uint8_t> rtcData = rtc.serialize();
    data.insert(data.end(), rtcData.begin(), rtcData.end());
    return data;
}

void MBC3::loadRam(const std::vector<uint8_t>& savedData) {
    size_t n = std::min(savedData.size(),eram.size());
    std::copy(savedData.begin(), savedData.begin() + static_cast<ptrdiff_t>(n), eram.begin());

    if(savedData.size() >= eram.size() + 13) {
        std::vector<uint8_t> rtcData(savedData.begin() + static_cast<ptrdiff_t>(eram.size()), savedData.end());
        rtc.deserialize(rtcData);
    }
}

void MBC3::tick(int cycles) {
    rtc.tick(cycles);
}