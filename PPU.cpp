#include "PPU.h"
#include<vector>
#include<algorithm>

uint8_t PPU::readByte(uint16_t addr) const {
    if (addr >= 0x8000 && addr <= 0x9FFF) return vram[addr - 0x8000];
    if (addr >= 0xFE00 && addr <= 0xFE9F) return oam[addr - 0xFE00];
    if (addr >= 0xFF40 && addr <= 0xFF4B) return lcd_regs[addr - 0xFF40];
    return 0xFF;
}

void PPU::writeByte(uint16_t addr, uint8_t val) {
    if (addr >= 0x8000 && addr <= 0x9FFF) vram[addr - 0x8000] = val;
    else if (addr >= 0xFE00 && addr <= 0xFE9F) oam[addr - 0xFE00] = val;
    else if (addr >= 0xFF40 && addr <= 0xFF4B) {
        uint16_t offset = addr - 0xFF40;
        if (offset == 1) {
            uint8_t cur = lcd_regs[1];
            uint8_t read = cur & 0x87; 
            uint8_t write = val & 0x78;    
            
            lcd_regs[1] = read | write;
        } 
        else if (offset == 4) {
            //LY is read only
        }
        else {
            lcd_regs[offset] = val;
        }
    }
}

bool PPU::isLCDEnabled() const{
    return (lcd_regs[0] & (1 << 7)) != 0; 
}

uint16_t PPU::getWindowTileMapArea() const{
    return ((lcd_regs[0] & (1 << 6)) != 0) ? 0x9C00 : 0x9800;
}

bool PPU::isWindowEnabled() const{
    return ((lcd_regs[0] & (1 << 5))) != 0;
}

uint16_t PPU::getTileDataArea() const{
    return (((lcd_regs[0] & (1 << 4))) != 0) ? 0x8000 : 0x8800;
}

uint16_t PPU::getBGTileMapArea() const{
    return (((lcd_regs[0] & (1 << 3))) != 0) ? 0x9C00 : 0x9800;
}

uint8_t PPU::getSpriteSize() const {
    return (lcd_regs[0] & (1 << 2)) != 0 ? 16 : 8; 
}

bool PPU::areSpritesEnabled() const {
    return (lcd_regs[0] & (1 << 1)) != 0;
}

bool PPU::isBackgroundEnabled() const {
    return (lcd_regs[0] & (1 << 0)) != 0;
}

bool PPU::isLYCInterruptEnabled() const {
    return (lcd_regs[1] & (1 << 6)) != 0;
}

bool PPU::isMode2InterruptEnabled() const {
    return (lcd_regs[1] & (1 << 5)) != 0;
}

bool PPU::isMode1InterruptEnabled() const {
    return (lcd_regs[1] & (1 << 4)) != 0;
}

bool PPU::isMode0InterruptEnabled() const {
    return (lcd_regs[1] & (1 << 3)) != 0;
}

bool PPU::getLYCCompareFlag() const {
    return (lcd_regs[1] & (1 << 2)) != 0;
}

uint8_t PPU::getPPUMode() const {
    return lcd_regs[1] & 0x03; 
}

void PPU::setPPUMode(uint8_t val) {
    lcd_regs[1] = static_cast<uint8_t>((lcd_regs[1] & ~0x03) | (val & 0x03));
}

void PPU::setLYCCompareFlag(bool match) {
    if(match) lcd_regs[1] |= static_cast<uint8_t>(1 << 2);
    else lcd_regs[1] &= static_cast<uint8_t>(~(1 << 2));
}

uint8_t PPU::step(uint8_t cycles) {
    if(!(lcd_regs[0] & 0x80)) return 0;
    uint8_t interrupt = 0;
    modeCounter += cycles;
    switch (mode){
        case 0: // HBlank
            if (modeCounter >= 204) {
                modeCounter -= 204;
                ly++;
                if(ly == 144) {
                    mode = 1;
                    interrupt |= (1 << 0); 
                    if(isMode1InterruptEnabled()) interrupt |= (1 << 1);
                }
                else {
                    mode = 2;
                    if(isMode2InterruptEnabled()) interrupt |= (1 << 1);
                }
            }
            break;

        case 1: // VBlank
            if (modeCounter >= 456) {
                modeCounter -= 456;
                ly++;
                if(ly > 153) { 
                    ly = 0; 
                    mode = 2; 
                    windowLine = 0;
                    if(isMode2InterruptEnabled()) interrupt |= (1 << 1);
                }
            }
            break;

        case 2: //OAM scan
            if(modeCounter >= 80) {
                modeCounter -= 80;
                mode = 3;
            }
            break;

         case 3: // Drawing
            if (modeCounter >= 172) {
                modeCounter -= 172;
                mode = 0;
                if (isMode0InterruptEnabled()) interrupt |= (1 << 1);
                drawScanline();
                drawSprites();
            }
            break;
        
        default:
            break;
    }

    lcd_regs[4] = ly;
    lcd_regs[1] = (lcd_regs[1] & 0xFC) | (mode & 0x03);

    uint8_t lyc = lcd_regs[5]; 
    bool wasMatch = getLYCCompareFlag();
    setLYCCompareFlag(lyc == ly);
    if((ly == lyc) && !wasMatch && isLYCInterruptEnabled()) {
        interrupt |= (1 << 1);
    }

    return interrupt;
}

void PPU::drawScanline() {
    if (!isBackgroundEnabled()) {
        for(int x = 0; x < 160; x++) {
            framebuffer[static_cast<size_t>(ly * 160 + x)] = 0; 
            bgColorId[static_cast<size_t>(x)] = 0;
        }
    }
    else {
        uint8_t scy = lcd_regs[2], scx = lcd_regs[3], bgp = lcd_regs[7];
        uint8_t wy = lcd_regs[10], wx = lcd_regs[11];
        uint16_t tileBase = getBGTileMapArea(), tileDataArea = getTileDataArea(), windowBase = getWindowTileMapArea();

        bool windowActive = isWindowEnabled() && (ly >= wy), windowDrewts = false;

        bool is_unsigned = ((lcd_regs[0] & (1 << 4)) != 0);

        for(uint8_t x = 0; x < 160; x++) {
            int wxAdj = static_cast<int>(wx) - 7;
            bool useWindow = windowActive && (x >= wxAdj);

            uint8_t bgX, bgY;
            uint16_t tileMapBase;

            if(useWindow) {
                tileMapBase = windowBase;
                bgX = static_cast<uint8_t>(x - (wx - 7));
                bgY = windowLine;
                windowDrewts = true;
            } 
            else{
                tileMapBase = tileBase;
                bgX = static_cast<uint8_t>(x + scx);
                bgY = static_cast<uint8_t>(ly + scy);
            }

            uint16_t addr = static_cast<uint16_t>(tileMapBase + (bgY/8)*32 + (bgX/8));
            uint8_t tileIndex = vram[addr - 0x8000];

            uint16_t tileAddr;
            if(is_unsigned) tileAddr = static_cast<uint16_t>(tileDataArea + tileIndex * 16);
            else tileAddr = static_cast<uint16_t>(0x9000 + static_cast<int8_t>(tileIndex) * 16);

            uint8_t row = bgY % 8;
            uint8_t byte1 = vram[static_cast<size_t>((tileAddr + row*2) - 0x8000)], byte2 = vram[static_cast<size_t>((tileAddr + row*2 + 1) - 0x8000)];

            uint8_t bit = 7 - (bgX % 8);
            uint8_t l = (byte1 >> bit) & 1, h = (byte2 >> bit) & 1;
            uint8_t colorId = static_cast<uint8_t>((h << 1) | l);
            uint8_t shade = (bgp >> (colorId * 2)) & 0x03;

            framebuffer[static_cast<size_t>(ly * 160 + x)] = shade;
            bgColorId[static_cast<size_t>(x)] = colorId;
        }
        if(windowDrewts) windowLine++;
    }

}

void PPU::drawSprites() {
    if(!areSpritesEnabled()) return;

    uint8_t spriteHeight = getSpriteSize();

    struct Sprite {
        uint8_t index, y, x, tile, attr;
    };

    std::vector<Sprite> line;
    for(int i = 0; i < 40 && line.size() < 10; i++) {
        uint8_t Y = oam[static_cast<size_t>(i * 4)], X = oam[static_cast<size_t>(i * 4 + 1)];
        int spriteY = static_cast<int>(Y) - 16;
        uint8_t tile = oam[static_cast<size_t>(i * 4 + 2)];
        uint8_t attr = oam[static_cast<size_t>(i * 4 + 3)];
        if(ly >= spriteY && ly < (spriteY + spriteHeight)) {
            line.push_back({
                static_cast<uint8_t>(i), 
                Y, 
                X,
                tile, 
                attr
            });
        }
    }

    std::sort(line.begin(), line.end(), [](const Sprite& a, const Sprite& b) {
        if (a.x != b.x) return a.x > b.x;           
        return a.index > b.index;            
    });

     for(const auto& s : line) {
        int spriteY = static_cast<int>(s.y) - 16;
        int spriteX = static_cast<int>(s.x) - 8;
        bool yFlip = s.attr & (1 << 6), xFlip = s.attr & (1 << 5);
        bool behindBG = s.attr & (1 << 7);
        uint8_t palette = (s.attr & (1 << 4)) ? lcd_regs[9] : lcd_regs[8];

        int row = ly - spriteY;
        if(yFlip) row = spriteHeight - 1 - row;

        uint8_t tileIndex = s.tile;
        if(spriteHeight == 16) tileIndex &= 0xFE;

        uint16_t tileAddr = static_cast<uint16_t>(0x8000 + tileIndex * 16 + row * 2);
        uint8_t byte1 = vram[static_cast<size_t>(tileAddr - 0x8000)];
        uint8_t byte2 = vram[static_cast<size_t>(tileAddr + 1 - 0x8000)];

        for(int px = 0; px < 8; px++) {
            int bit = xFlip ? px : (7 - px);
            uint8_t l = (byte1 >> bit) & 1, h = (byte2 >> bit) & 1;
            uint8_t colorId = static_cast<uint8_t>((h << 1) | l);
            if (colorId == 0) continue;

            int screenX = spriteX + px;
            if (screenX < 0 || screenX >= 160) continue;
            if (behindBG && bgColorId[static_cast<size_t>(screenX)] != 0) continue;

            uint8_t shade = (palette >> (colorId * 2)) & 0x03;
            framebuffer[static_cast<size_t>(ly * 160 + screenX)] = shade;
        }
     }
}

