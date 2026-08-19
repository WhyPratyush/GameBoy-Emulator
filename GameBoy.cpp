#include "GameBoy.h"

GameBoy::GameBoy() : cpu(&mmu) {
    totalCycles = 0;
}

void GameBoy::loadRom(const std::vector<uint8_t>& rom_data) {
    mmu.loadRom(rom_data);
}

uint16_t GameBoy::getPC() { return cpu.get_pc(); }
bool GameBoy::isHalted() { return cpu.is_halted(); }
bool GameBoy::getIME() { return cpu.get_ime(); }

bool GameBoy::pollFrameReady() {
    bool r = isReady;
    isReady = false;
    return r;
}

const std::array<uint8_t, 160*144>& GameBoy::getFrameBuffer() {
    return mmu.getFrameBuffer();
}

uint8_t GameBoy::stepInstruction() {
    uint8_t interruptCycles = cpu.interrupt_handler();
    if(interruptCycles > 0) {
        totalCycles += interruptCycles;
        mmu.update_timers(interruptCycles);
        if(mmu.update_ppu(interruptCycles)) isReady = true;
        return interruptCycles;
    }

    if(cpu.is_halted()) {
        totalCycles += 4;
        mmu.update_timers(4);
        if(mmu.update_ppu(4)) isReady = true;
        return 4;
    }

    uint8_t opcode = cpu.fetch();
    uint8_t cycles = cpu.execute(opcode);
    
    totalCycles += cycles;
    mmu.update_timers(cycles);
    if(mmu.update_ppu(cycles)) isReady = true;
    
    return cycles;
}

void GameBoy::stepFrame() {
    int cyclesThisFrame = 0;
    while (cyclesThisFrame < MAX_CYCLES_PER_FRAME) cyclesThisFrame += stepInstruction();
}

void GameBoy::run() {
    while (true) {
        stepFrame();
    }
}

void GameBoy::joypad(SDL_Keycode key, bool isKeyUp) {
    uint8_t bit = isKeyUp ? 1 : 0;
    uint8_t previousState = mmu.readByte(0xFF00);
    
    switch(key) {
        case SDLK_d: mmu.dirKeys = static_cast<uint8_t>((mmu.dirKeys & ~(1<<0)) | (bit<<0)); break; // Right
        case SDLK_a: mmu.dirKeys = static_cast<uint8_t>((mmu.dirKeys & ~(1<<1)) | (bit<<1)); break; // Left
        case SDLK_w: mmu.dirKeys = static_cast<uint8_t>((mmu.dirKeys & ~(1<<2)) | (bit<<2)); break; // Up
        case SDLK_s: mmu.dirKeys = static_cast<uint8_t>((mmu.dirKeys & ~(1<<3)) | (bit<<3)); break; // Down
        
        case SDLK_z: mmu.actionKeys = static_cast<uint8_t>((mmu.actionKeys & ~(1<<0)) | (bit<<0)); break; // A
        case SDLK_x: mmu.actionKeys = static_cast<uint8_t>((mmu.actionKeys & ~(1<<1)) | (bit<<1)); break; // B
        case SDLK_SPACE: mmu.actionKeys = static_cast<uint8_t>((mmu.actionKeys & ~(1<<2)) | (bit<<2)); break; // Select
        case SDLK_RETURN: mmu.actionKeys = static_cast<uint8_t>((mmu.actionKeys & ~(1<<3)) | (bit<<3)); break; // Start
    }
    uint8_t newState = mmu.readByte(0xFF00);
    
    if((previousState & ~newState) & 0x0F) {
        mmu.requestInterrupt(4); 
    }
}