#pragma once
#include <cstdint>
#include <array>

class PPU {
private:
    std::array<uint8_t, 8192> vram{};   
    std::array<uint8_t, 160> oam{};     
    std::array<uint8_t, 12> lcd_regs{};
    std::array<uint8_t, 160*144> framebuffer{}; 
    std::array<uint8_t, 160> bgColorId{}; 
    void drawScanline(); 
    void drawSprites();

    uint16_t modeCounter = 0; 
    uint8_t mode = 2;        
    uint8_t ly = 0;
    uint8_t windowLine = 0;

public:
    uint8_t step(uint8_t cycles);
    uint8_t readByte(uint16_t addr) const;
    void writeByte(uint16_t addr, uint8_t val);
    bool isLCDEnabled() const;
    uint16_t getWindowTileMapArea() const;
    bool isWindowEnabled() const;
    uint16_t getTileDataArea() const;
    uint16_t getBGTileMapArea() const;
    uint8_t getSpriteSize() const;
    bool areSpritesEnabled() const;
    bool isBackgroundEnabled() const;
    bool isLYCInterruptEnabled() const;
    bool isMode2InterruptEnabled() const;
    bool isMode1InterruptEnabled() const;
    bool isMode0InterruptEnabled() const;
    bool getLYCCompareFlag() const;
    const std::array<uint8_t, 160*144>& getFrameBuffer() const { return framebuffer; }
    uint8_t getPPUMode() const;
    void setPPUMode(uint8_t mode);
    void setLYCCompareFlag(bool match);
};