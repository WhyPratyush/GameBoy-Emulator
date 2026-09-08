#pragma once
#include <cstdint>
#include <chrono>
#include <vector>


class RTCregs{
    public:
        uint8_t rtc_s = 0;
        uint8_t rtc_m = 0;
        uint8_t rtc_h = 0;
        uint8_t rtc_dl = 0;
        uint8_t rtc_dh = 0;
};

class RTC{
private:
    RTCregs live, latched;
    bool latchReady = false;
    int cycleCounter = 0;

    void advance1sec();

public:
    RTC() = default;
    void writeLatch(uint8_t val);
    uint8_t readReg(uint8_t reg) const;
    void writeReg(uint8_t reg, uint8_t val);
    void tick(int cycles);
    std::vector<uint8_t> serialize() const;
    void deserialize(const std::vector<uint8_t>& data);
    
};