#pragma once
#include "CPU.h"
#include "MMU.h"
#include <vector>
#include <cstdint>

class GameBoy {
private:
    MMU mmu;
    CPU cpu;
    const int MAX_CYCLES_PER_FRAME = 70224;
    uint64_t totalCycles; 

public:
    GameBoy();
    void loadRom(const std::vector<uint8_t>& rom_data);
    uint8_t stepInstruction(); 
    void stepFrame();          
    void run();                
};