#pragma once
#include <cstdint>
#include <vector>

class MBC {
public:
    virtual ~MBC() = default;
    
    virtual uint8_t readByte(uint16_t address) = 0;
    virtual void writeByte(uint16_t address, uint8_t value) = 0;
};