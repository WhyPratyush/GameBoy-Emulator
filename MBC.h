#pragma once
#include <cstdint>
#include <vector>

class MBC {
protected:
    bool battery = false;
    bool isDirty = false;
public:
    virtual ~MBC() = default;
    bool pollDirty() {
        bool cur = isDirty;
        isDirty = false;
        return cur;
    }
    bool hasBattery() const {return battery;};

    virtual const std::vector<uint8_t> getRam() const = 0;
    virtual void loadRam(const std::vector<uint8_t>& savedData) = 0;
    virtual void tick(int cycles) {};
    
    virtual uint8_t readByte(uint16_t address) = 0;
    virtual void writeByte(uint16_t address, uint8_t value) = 0;
};