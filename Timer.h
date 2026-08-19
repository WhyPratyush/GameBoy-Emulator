#pragma once
#include <cstdint>

class Timer {
private:
    uint8_t div = 0;
    uint8_t tima = 0;
    uint8_t tma = 0;
    uint8_t tac = 0;

    int divCounter = 0;
    int timaCounter = 0;

    int getClockThreshold();

public:
    bool tick(int cycles);
    uint8_t readByte(uint16_t addr) const;
    void writeByte(uint16_t addr, uint8_t val);
};