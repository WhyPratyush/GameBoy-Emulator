#include "Timer.h"

int Timer::getClockThreshold() {
    switch (tac & 0b11) {
        case 0b00: return 1024; // 4096 Hz
        case 0b01: return 16;   // 262144 Hz
        case 0b10: return 64;   // 65536 Hz
        case 0b11: return 256;  // 16384 Hz
    }
    return 1024;
}

void Timer::tick(int cycles) {
    divCounter += cycles;
    while (divCounter >= 256) {
        divCounter -= 256;
        div++;
    }

    if (tac & 0b100) {
        timaCounter += cycles;
        int threshold = getClockThreshold();

        while (timaCounter >= threshold) {
            timaCounter -= threshold;
            if (tima == 0xFF) {
                tima = tma; 
                // TODO: Request Timer Interrupt (Set bit 2 in IF register)
            } 
            else tima++;
        }
    }
}

uint8_t Timer::readByte(uint16_t addr) const {
    switch (addr) {
        case 0xFF04: return div;
        case 0xFF05: return tima;
        case 0xFF06: return tma;
        case 0xFF07: return tac;
        default: return 0xFF;
    }
}

void Timer::writeByte(uint16_t addr, uint8_t val) {
    switch (addr) {
        case 0xFF04: 
            div = 0;
            divCounter = 0; 
            break;
        case 0xFF05: 
            tima = val; 
            break;
        case 0xFF06: 
            tma = val; 
            break;
        case 0xFF07: 
            tac = val; 
            break;
    }
}