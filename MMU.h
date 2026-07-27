#pragma once

#include<array>
#include<vector>
#include<cstdint>
#include "Timer.h"

class MMU{
    private:
        std::vector<uint8_t> rom;        // 32 kb rom
        std::array<uint8_t,8192> vram{}; // 8 kb video ram
        std::array<uint8_t,8192> eram{}; // 8 kb external ram
        std::array<uint8_t,8192> wram{}; // 8 kb work ram
        std::array<uint8_t,160> oam{};   // Object attribute memory
        std::array<uint8_t,128> ioreg{}; // I/O registers
        std::array<uint8_t,127> hram{};  // High ram
        uint8_t ieReg = 0;               // Input Enable Register   
        Timer timer;
    
    public:
        void update_timers(int cycles) {
            timer.tick(cycles);
        }

        void loadRom(const std::vector<uint8_t>& data) {
            //Load the rom from the data, don't need the filestream here
            rom = data;
        }

        uint8_t readByte(uint16_t addr) const {
            if(addr >= 0xFF04 && addr <= 0xFF07) {
                return timer.readByte(addr);
            }

            //ROM
            if(addr <= 0x7FFF) {
                if(addr < rom.size()) return rom[addr];
                else return 0xFF;
            }

            //VRAM
            if(addr <= 0x9FFF) return vram[addr-0x8000];

            //External RAM
            if(addr <= 0xBFFF) return eram[addr-0xA000];

            //Work RAM
            if(addr <= 0xDFFF) return wram[addr-0xC000];

            //Echo RAM - Mirror of WRAM so just return that
            if(addr <= 0xFDFF) return wram[addr-0xE000];

            //OAM
            if(addr <= 0xFE9F) return oam[addr-0xFE00];

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

            if(addr >= 0xFF04 && addr <= 0xFF07) {
                timer.writeByte(addr, val);
                return; 
            }
            
            //ROM
            if(addr <= 0x7FFF) {
                //Read Only ROM
                //Later will implement the MBC bank switching for larger ROMs
                return;
            }

            //VRAM
            if(addr <= 0x9FFF) {
                vram[addr-0x8000] = val;
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

            //OAM
            if(addr <= 0xFE9F) {
                oam[addr-0xFE00] = val;
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
};