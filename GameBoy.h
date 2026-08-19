#pragma once
#include "CPU.h"
#include "MMU.h"
#include<vector>
#include<cstdint>
#include<array>
#include<SDL2/SDL.h>

class GameBoy {
private:
    MMU mmu;
    CPU cpu;
    const int MAX_CYCLES_PER_FRAME = 70224;
    uint64_t totalCycles; 
    bool isReady = false;

public:
    GameBoy();
    uint16_t getPC();
    bool isHalted();
    bool getIME();
    void loadRom(const std::vector<uint8_t>& rom_data);
    uint8_t stepInstruction(); 
    void stepFrame();          
    void run();                
    bool pollFrameReady();
    void joypad(SDL_Keycode key, bool isKeyUp);
    const std::array<uint8_t, 160*144>& getFrameBuffer();
};