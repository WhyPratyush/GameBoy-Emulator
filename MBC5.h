#pragma once
#include "MBC.h"

class MBC5 : public MBC {
private:
    uint16_t romBank = 1;
    uint8_t ramBank = 0;
    bool ramEnabled = false;
    bool hasRumble = false;

    const std::vector<uint8_t>& rom;
    std::vector<uint8_t> eram; 

public:
    MBC5(const std::vector<uint8_t>& romData, size_t ramSize,bool rumble = false) : rom(romData), eram(ramSize, 0xFF), hasRumble(rumble) {}

    uint8_t readByte(uint16_t addr) override;
    void writeByte(uint16_t addr, uint8_t val) override;
    const std::vector<uint8_t> getRam() const override;
    void loadRam(const std::vector<uint8_t>& savedData) override;
};