#pragma once

#include<iostream>
#include<array>
#include<vector>
#include<cstdint>
#include "Timer.h"
#include "PPU.h"

class MMU{
    private:
        std::vector<uint8_t> rom;        // 32 kb rom
        std::array<uint8_t,8192> eram{}; // 8 kb external ram
        std::array<uint8_t,8192> wram{}; // 8 kb work ram
        std::array<uint8_t,128> ioreg{}; // I/O registers
        std::array<uint8_t,127> hram{};  // High ram
        uint8_t ieReg = 0;               // Input Enable Register   
        Timer timer;
        uint8_t intFlag = 0xE0;            //Interrupt flag
        PPU ppu;
        
    public:
        uint8_t actionKeys = 0x0F; 
        uint8_t dirKeys = 0x0F;
        uint8_t joypadReg = 0xFF;

        void update_timers(int cycles) {
            if(timer.tick(cycles)) {
                requestInterrupt(2);
            }
        }

        void loadRom(const std::vector<uint8_t>& data) {
            //Load the rom from the data, don't need the filestream here
            rom = data;
        }

        bool update_ppu(int cycles) {
            uint8_t interrupts = ppu.step(static_cast<uint8_t>(cycles));
            if (interrupts & (1 << 0)) requestInterrupt(0); 
            if (interrupts & (1 << 1)) requestInterrupt(1);
            return (interrupts & (1 << 0)) != 0;
        }

        uint8_t readByte(uint16_t addr) const {

            if(addr == 0xFF00) {
                uint8_t result = joypadReg | 0xCF; 

                if ((joypadReg & (1 << 4)) == 0) result &= (dirKeys | 0xF0);
                if ((joypadReg & (1 << 5)) == 0) result &= (actionKeys | 0xF0);
                
                return result;
            }

            if(addr == 0xFF0F) {
                return intFlag;
            }

            if(addr >= 0xFF04 && addr <= 0xFF07) {
                return timer.readByte(addr);
            }

            //ROM
            if(addr <= 0x7FFF) {
                if(addr < rom.size()) return rom[addr];
                else return 0xFF;
            }

            if ((addr >= 0x8000 && addr <= 0x9FFF) || (addr >= 0xFE00 && addr <= 0xFE9F) || (addr >= 0xFF40 && addr <= 0xFF4B)) {
                return ppu.readByte(addr);
            }

            //External RAM
            if(addr <= 0xBFFF) return eram[addr-0xA000];

            //Work RAM
            if(addr <= 0xDFFF) return wram[addr-0xC000];

            //Echo RAM - Mirror of WRAM so just return that
            if(addr <= 0xFDFF) return wram[addr-0xE000];

            //Unsuable region
            if(addr <= 0xFEFF) return 0xFF;

            //I/O regs
            if(addr <= 0xFF7F) return ioreg[addr-0xFF00];

            //HRAM
            if(addr <= 0xFFFE) return hram[addr-0xFF80];

            //Interrupt Enable Reg
            return  ieReg;
        }

        void writeByte(uint16_t addr, uint8_t val) {

            if(addr == 0xFF00) {
                joypadReg = (joypadReg & 0xCF) | (val & 0x30);
                return;
            }

            if(addr == 0xFF0F) {
                intFlag = static_cast<uint8_t>(val | 0xE0); 
                return;
            }

            if(addr == 0xFF46) {
                uint16_t sourceAddr = static_cast<uint16_t>(val << 8); 
                
                for(uint16_t i = 0; i < 160; i++) {
                    uint8_t byte = readByte(sourceAddr + i);
                    ppu.writeByte(0xFE00 + i, byte);
                }
                return; 
            }

            if(addr >= 0xFF04 && addr <= 0xFF07) {
                timer.writeByte(addr, val);
                return; 
            }

            if(addr == 0xFF02 && val == 0x81) {
                char c = static_cast<char>(readByte(0xFF01));
                std::cout << c;
                std::cout.flush();
            }

            
            
            //ROM
            if(addr <= 0x7FFF) {
                //Read Only ROM
                //Later will implement the MBC bank switching for larger ROMs
                return;
            }

            //VRAM
            if((addr >= 0x8000 && addr <= 0x9FFF) || (addr >= 0xFE00 && addr <= 0xFE9F) || (addr >= 0xFF40 && addr <= 0xFF4B)) {
                ppu.writeByte(addr, val);
                return;
            }

            //External RAM
            if(addr <= 0xBFFF) {
                eram[addr-0xA000] = val; 
                return; 
            }

            //Work RAM
            if(addr <= 0xDFFF) {
                wram[addr-0xC000] = val;
                return;
            }

            //Echo RAM - Mirror of WRAM so just return that
            if(addr <= 0xFDFF) {
                wram[addr-0xE000] = val;
                return;
            }

            //Unsuable region
            if(addr <= 0xFEFF) return;

            //I/O regs
            if(addr <= 0xFF7F) {
                ioreg[addr-0xFF00] = val;
                return; 
            }

            //HRAM
            if(addr <= 0xFFFE) {
                hram[addr-0xFF80] = val;
                return; 
            }

            //Interrupt Enable Reg
            ieReg = val;
        }

        uint16_t readWord(uint16_t addr) const {
            uint8_t low = readByte(addr);
            uint8_t high = readByte(static_cast<uint16_t>(addr+1));
            return static_cast<uint16_t>((high << 8) | low);
        }

        void writeWord(uint16_t addr, uint16_t val) {
            uint8_t val1 = static_cast<uint8_t>(val & 0xFF); //value at lower byte
            uint8_t val2 = static_cast<uint8_t>((val >> 8) & 0xFF); //value at higher byte
            writeByte(addr,val1);
            writeByte(static_cast<uint16_t>(addr+1),val2);
        }

        void requestInterrupt(uint8_t bit) {
            intFlag |= static_cast<uint8_t>(1 << bit);
        }

        const std::array<uint8_t, 160*144>& getFrameBuffer() const {
            return ppu.getFrameBuffer();
        }
};