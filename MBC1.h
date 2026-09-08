#pragma once
#include "MBC.h"

class MBC1 : public MBC {
private:
    uint8_t romBank = 1;
    uint8_t ramBank = 0;
    bool ramEnabled = false;
    bool bankingMode = false; 

    const std::vector<uint8_t>& rom;
    std::vector<uint8_t> eram; 

public:
    MBC1(const std::vector<uint8_t>& romData,bool hasBattery) : rom(romData), eram(32*1024, 0xFF) {
        battery = hasBattery;
    }
    
    uint8_t readByte(uint16_t addr) override;
    void writeByte(uint16_t addr, uint8_t val) override;
    const std::vector<uint8_t> getRam() const override;
    void loadRam(const std::vector<uint8_t>& savedData) override;
};