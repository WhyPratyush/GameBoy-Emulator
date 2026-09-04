#pragma once
#include "MBC.h"
#include<vector>
#include<cstdint>

class MBC3 : public MBC {
private:
    const std::vector<uint8_t>& rom;
    std::vector<uint8_t> eram;

    uint16_t romBank = 1; 
    uint8_t ramRtcSelect = 0, romMask;
    bool ramRtcEnabled = false;

public:
    MBC3(const std::vector<uint8_t>& romData, bool hasBattery, bool isMBC30 = false) 
        : rom(romData), eram(64 * 1024, 0xFF),romMask(isMBC30 ? 0xFF: 0x7F)  {
            battery = hasBattery;
        };
    uint8_t readByte(uint16_t addr) override;
    void writeByte(uint16_t addr, uint8_t val) override;

    const std::vector<uint8_t>& getRam() const override;
    void loadRam(const std::vector<uint8_t>& savedData) override;   
};