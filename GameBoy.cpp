#include "GameBoy.h"

GameBoy::GameBoy() : cpu(&mmu) {
    totalCycles = 0;
}

void GameBoy::loadRom(const std::vector<uint8_t>& rom_data) {
    mmu.loadRom(rom_data);
}

uint8_t GameBoy::stepInstruction() {
    uint8_t opcode = cpu.fetch();
    uint8_t cycles = cpu.execute(opcode);
    
    totalCycles += cycles;
    mmu.update_timers(cycles);
    
    // Future: 
    // cpu.handle_interrupts();
    
    return cycles;
}

void GameBoy::stepFrame() {
    int cyclesThisFrame = 0;
    
    while (cyclesThisFrame < MAX_CYCLES_PER_FRAME) {
        cyclesThisFrame += stepInstruction();
    }
    
    // Future: 
    // ppu.render_frame();
}

void GameBoy::run() {
    while (true) {
        stepFrame();
        // Vsync or sleep logic 
    }
}