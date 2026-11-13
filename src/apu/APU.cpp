#include "APU.h"

APU::APU() {
    ch1.NR1x = {0x80, 0xBF, 0xF3, 0xFF, 0xBF}; 
    ch2.NR2x = {0x3F, 0x00, 0xFF, 0xBF};      
    ch3.NR3x = {0x7F, 0xFF, 0x9F, 0xFF, 0xBF}; 
    ch4.NR4x = {0xFF, 0x00, 0x00, 0xBF}; 
    NR5x = {0x77, 0xF3, 0x80}; 
    ch1.enable = true;
}

int APU::noisePeriod() const {
    uint8_t divCode = ch4.NR4x[2] & 0x07;
    uint8_t shift = (ch4.NR4x[2] >> 4) & 0x0F;
    int divisor = (divCode == 0) ? 8 : divCode * 16;
    return divisor << shift;
}

void APU::lengthWrite(int& lengthTimer, bool& enable, bool wasLenEnabled, uint8_t val, int maxLen) {
    bool lenEnabled = (val & 0x40) != 0;
    bool trigger = (val & 0x80) != 0;
    bool extra = (frameSequencer & 1) != 0;  

    if(!wasLenEnabled && lenEnabled && extra && lengthTimer > 0) {
        lengthTimer--;
        if(lengthTimer == 0 && !trigger) enable = false;
    }
    if(trigger && lengthTimer == 0) {
        lengthTimer = maxLen;
        if(lenEnabled && extra) lengthTimer--;
    }
}

int APU::sweepCalculate() {
    int offset = ch1.freqShadowReg >> (ch1.NR1x[0] & 0x07);
    if(ch1.NR1x[0] & 0x08) { 
        ch1.negatedCalc = true; 
        return ch1.freqShadowReg - offset;
    }
    return ch1.freqShadowReg + offset;
}

uint8_t APU::readByte(uint16_t addr) const {
    if(addr >= 0xFF30 && addr <= 0xFF3F) {
        if(ch3.enable) return waveAccessible() ? ch3.waveRAM[static_cast<size_t>(ch3.samplePosition/2)] : 0xFF;
        return ch3.waveRAM[addr - 0xFF30];
    }
    if(addr == 0xFF26) {
        uint8_t res = NR5x[2] & 0x80; 
        res |= 0x70;
        if(ch1.enable) res |= 0x01;
        if(ch2.enable) res |= 0x02;
        if(ch3.enable) res |= 0x04;
        if(ch4.enable) res |= 0x08;
        return res;
    }
    if(addr == 0xFF25) return NR5x[1];
    if(addr == 0xFF24) return NR5x[0];
    if(addr == 0xFF23) return (ch4.NR4x[3] & 0x40) | 0xBF;
    if(addr == 0xFF22) return ch4.NR4x[2];
    if(addr == 0xFF21) return ch4.NR4x[1];
    if(addr == 0xFF1E) return (ch3.NR3x[4] & 0x40) | 0xBF; 
    if(addr == 0xFF1C) return ch3.NR3x[2] | 0x9F;             
    if(addr == 0xFF1A) return ch3.NR3x[0] | 0x7F;
    if(addr == 0xFF19) return (ch2.NR2x[3] & 0x40) | 0xBF; 
    if(addr == 0xFF17) return ch2.NR2x[1];
    if(addr == 0xFF16) return (ch2.NR2x[0] & 0xC0) | 0x3F;
    if(addr == 0xFF14) return (ch1.NR1x[4] & 0x40) | 0xBF;
    if(addr == 0xFF11) return (ch1.NR1x[1] & 0xC0) | 0x3F;
    if(addr == 0xFF12) return ch1.NR1x[2];
    if(addr == 0xFF10) return ch1.NR1x[0] | 0x80;
    // NR13, NR23, NR31, NR33, NR41 are write-only
    return 0xFF;
}

void APU::writeByte(uint16_t addr, uint8_t val) {
    bool powered = (NR5x[2] & 0x80) != 0;
    if(!powered && addr != 0xFF26 && !(addr >= 0xFF30 && addr <= 0xFF3F) 
        && addr != 0xFF11 && addr != 0xFF16 && addr != 0xFF1B && addr != 0xFF20) return;
    if(addr >= 0xFF30 && addr <= 0xFF3F) {
        if(ch3.enable) {
            if(waveAccessible()) ch3.waveRAM[static_cast<size_t>(ch3.samplePosition/2)] = val;
            return;
        }
        ch3.waveRAM[addr - 0xFF30] = val;
        return;
    }
    if(addr == 0xFF26) {
        bool wasOn = (NR5x[2] & 0x80) != 0;
        bool turnOn = (val & 0x80) != 0;
        NR5x[2] = (val & 0x80); 

        if(wasOn && !turnOn) {
            ch1.NR1x.fill(0);
            ch2.NR2x.fill(0);
            ch3.NR3x.fill(0);
            ch4.NR4x.fill(0);
            
            NR5x[0] = 0;
            NR5x[1] = 0;
            
            ch1.enable = ch2.enable = ch3.enable = ch4.enable = false;
        }
        else if(!wasOn && turnOn) {
            frameSequencer = 0; 
            audioTimer = 0;
        }
        return;
    }
    if(addr == 0xFF25){
        NR5x[1] = val; 
        return;
    }
    if(addr == 0xFF24) {
        NR5x[0] = val; 
        return; 
    }
    if(addr == 0xFF23) {
        bool wasLen = (ch4.NR4x[3] & 0x40) != 0;
        ch4.NR4x[3] = val;
        lengthWrite(ch4.lengthTimer, ch4.enable, wasLen, val, 64);  
        if(val & 0x80) {
            ch4.enable = (ch4.NR4x[1] & 0xF8) != 0;
            ch4.freqTimer = noisePeriod();
            ch4.volume = (ch4.NR4x[1] >> 4) & 0x0F;
            if(ch4.lengthTimer == 0) ch4.lengthTimer = 64;
            uint8_t sweepPace = ch4.NR4x[1] & 0x07;
            ch4.envelopeTimer = (sweepPace == 0) ? 8 : sweepPace;
            ch4.lfsr = 0x7FFF;
        }
        return;
    }
    if(addr == 0xFF22) {
        ch4.NR4x[2] = val;
        return;
    }
    if(addr == 0xFF21) {
        ch4.NR4x[1] = val;
        if((val & 0xF8) == 0) ch4.enable = false;
        return;
    }
    if(addr == 0xFF20) {
        ch4.NR4x[0] = val;
        ch4.lengthTimer = 64 - (val & 0x3F); 
        return;
    }
    if(addr == 0xFF1E) {
        bool wasOn = ch3.enable;
        bool aboutToRead = ch3.enable && ch3.freqTimer <= 2;
        bool wasLen = (ch3.NR3x[4] & 0x40) != 0;
        ch3.NR3x[4] = val;
        lengthWrite(ch3.lengthTimer, ch3.enable, wasLen, val, 256);   
        if(val & 0x80) {
            if(wasOn && aboutToRead) {
                int pos = ((ch3.samplePosition + 1) & 31) >> 1;
                if(pos < 4) ch3.waveRAM[0] = ch3.waveRAM[static_cast<size_t>(pos)];
                else {
                    int base = pos & ~3;
                    for(int i = 0; i < 4; i++) ch3.waveRAM[static_cast<size_t>(i)] = ch3.waveRAM[static_cast<size_t>(base + i)];
                }
            }
            ch3.enable = (ch3.NR3x[0] & 0x80) != 0;
            uint16_t periodValue = static_cast<uint16_t>(ch3.NR3x[3] | ((val & 0x07) << 8));
            ch3.freqTimer = ((2048 - periodValue)<<1) + 6;
            ch3.samplePosition = 0;
        }
        return;
    }
    if(addr == 0xFF1D) {
        ch3.NR3x[3] = val; 
        return;
    }
    if(addr == 0xFF1C) {
        ch3.NR3x[2] = val;
        ch3.outputLevel = (val & 0x60)>>5; 
        return;
    }
    if(addr == 0xFF1B) {
        ch3.NR3x[1] = val;
        ch3.lengthTimer = 256 - val; 
        return;
    }
    if(addr == 0xFF1A) {
        ch3.NR3x[0] = val;
        if((val & 0x80) == 0) ch3.enable = false;
        return;
    }
    if(addr == 0xFF19) {
        bool wasLen = (ch2.NR2x[3] & 0x40) != 0;
        ch2.NR2x[3] = val;
        lengthWrite(ch2.lengthTimer, ch2.enable, wasLen, val, 64);  
        if(val & 0x80) { 
            ch2.enable = (ch2.NR2x[1] & 0xF8) != 0;
            uint16_t periodVal = static_cast<uint16_t>(ch2.NR2x[2] | ((val & 0x07) << 8));
            ch2.freqTimer = (2048 - periodVal)<<2;
            uint8_t sweepPace = ch2.NR2x[1] & 0x07;
            ch2.envelopeTimer = (sweepPace == 0) ? 8 : sweepPace;
            ch2.volume = (ch2.NR2x[1]>>4) & 0x0F;
        }
        return;
    }
    if(addr == 0xFF18) {
        ch2.NR2x[2] = val;
        return;
    }
    if(addr == 0xFF17) {
        ch2.NR2x[1] = val;
        if((val & 0xF8) == 0) ch2.enable = false;
        return;
    }
    if(addr == 0xFF16) {
        if(powered) ch2.NR2x[0] = val;
        ch2.lengthTimer = 64 - (val & 0x3F);
        return;
    }
    if(addr == 0xFF14) {
        bool wasLen = (ch1.NR1x[4] & 0x40) != 0;
        ch1.NR1x[4] = val;
        lengthWrite(ch1.lengthTimer, ch1.enable, wasLen, val, 64);   
        if(val & 0x80) {
            ch1.enable = (ch1.NR1x[2] & 0xF8) != 0;
            uint16_t periodVal = static_cast<uint16_t>(ch1.NR1x[3] | ((val & 0x07) << 8));
            ch1.freqTimer = (2048 - periodVal) << 2;
            uint8_t sweepPace = ch1.NR1x[2] & 0x07;
            ch1.envelopeTimer = (sweepPace == 0) ? 8 : sweepPace;
            ch1.volume = (ch1.NR1x[2] >> 4) & 0x0F;
            ch1.freqShadowReg = periodVal;
            uint8_t sweepPaceNr10 = (ch1.NR1x[0]>>4) & 0x07;
            uint8_t shift = ch1.NR1x[0] & 0x07;
            ch1.sweepTimer = (sweepPaceNr10 == 0) ? 8 : sweepPaceNr10; 
            //don't question it, idk how i did ts
            ch1.sweepEnabled = (sweepPaceNr10 != 0 || shift != 0); 
            ch1.negatedCalc = false;
            if(shift != 0) {
                ch1.negatedCalc = false;
                if(shift != 0 && sweepCalculate() > 2047) ch1.enable = false;
            }
        }
        return;
    }
    if(addr == 0xFF13) {
        ch1.NR1x[3] = val;
        return;
    }
    if(addr == 0xFF12) {
        ch1.NR1x[2] = val;
        if((val & 0xF8) == 0) ch1.enable = false;
        return;
    }
    if(addr == 0xFF11) {
        if(powered) ch1.NR1x[1] = val;
        ch1.lengthTimer = 64 - (val & 0x3F);
        return;
    }
    if(addr == 0xFF10) {
        if(ch1.negatedCalc && !(val & 0x08)) ch1.enable = false;
        ch1.NR1x[0] = val;
        return;
    }
}

void APU::tick(int cycles) {
    if(NR5x[2] & 0x80) {
        audioTimer += cycles;
        while(audioTimer >= 8192) {
            audioTimer -= 8192;
            if(!(frameSequencer&1)) {
                if(ch1.lengthTimer > 0 && (ch1.NR1x[4] & 0x40)) {
                    ch1.lengthTimer--;
                    if(ch1.lengthTimer == 0) ch1.enable = false;
                }
                if(ch2.lengthTimer > 0 && (ch2.NR2x[3] & 0x40)) {
                    ch2.lengthTimer--;
                    if(ch2.lengthTimer == 0) ch2.enable = false;
                }
                if(ch3.lengthTimer > 0 && (ch3.NR3x[4] & 0x40)) {
                    ch3.lengthTimer--;
                    if(ch3.lengthTimer == 0) ch3.enable = false;
                }
                if(ch4.lengthTimer > 0 && (ch4.NR4x[3] & 0x40)) {
                    ch4.lengthTimer--;
                    if(ch4.lengthTimer == 0) ch4.enable = false;
                }
            }

            if(frameSequencer == 7) {
                uint8_t pace1 = ch1.NR1x[2] & 0x07;
                if(pace1 != 0) {
                    if(ch1.envelopeTimer > 0) ch1.envelopeTimer--;
                    if(ch1.envelopeTimer == 0) {
                        ch1.envelopeTimer = pace1;
                        uint8_t dir = (ch1.NR1x[2] >> 3) & 0x01;
                        if(dir == 1 && ch1.volume < 15) ch1.volume++;
                        else if(dir == 0 && ch1.volume > 0) ch1.volume--; //help i'm going insane
                    }
                }
                
                uint8_t pace2 = ch2.NR2x[1] & 0x07;
                if(pace2 != 0) {
                    if(ch2.envelopeTimer > 0) ch2.envelopeTimer--;
                    if(ch2.envelopeTimer == 0) {
                        ch2.envelopeTimer = pace2;
                        uint8_t dir = (ch2.NR2x[1] >> 3) & 0x01;
                        if(dir == 1 && ch2.volume < 15) ch2.volume++;
                        else if(dir == 0 && ch2.volume > 0) ch2.volume--;
                    }
                }

                uint8_t pace4 = ch4.NR4x[1] & 0x07;
                if(pace4 != 0) {
                    if(ch4.envelopeTimer > 0) ch4.envelopeTimer--;
                    if(ch4.envelopeTimer == 0) {
                        ch4.envelopeTimer = pace4;
                        uint8_t dir = (ch4.NR4x[1] >> 3) & 0x01;
                        if(dir == 1 && ch4.volume < 15) ch4.volume++;
                        else if(dir == 0 && ch4.volume > 0) ch4.volume--;
                    }
                }
            }
            if(ch1.enable && (frameSequencer == 2 || frameSequencer == 6)) {
                if(ch1.sweepTimer > 0) ch1.sweepTimer--;
                
                if(ch1.sweepTimer == 0) {
                    uint8_t sweepPace = (ch1.NR1x[0] >> 4) & 0x07;
                    ch1.sweepTimer = (sweepPace == 0) ? 8 : sweepPace;
                    if(ch1.sweepEnabled && sweepPace != 0) {
                        int newFreq = sweepCalculate();
                        if(newFreq > 2047) ch1.enable = false;
                        else if((ch1.NR1x[0] & 0x07) != 0) {
                            ch1.freqShadowReg = newFreq;
                            ch1.NR1x[3] = static_cast<uint8_t>(newFreq & 0xFF);
                            ch1.NR1x[4] = static_cast<uint8_t>((ch1.NR1x[4] & 0xF8) | ((newFreq >> 8) & 0x07));
                            if(sweepCalculate() > 2047) ch1.enable = false;  
                        }
                    }
                }
            }
            frameSequencer = static_cast<uint8_t>((frameSequencer + 1) % 8);
        }

        if(ch1.enable) {
            ch1.freqTimer -= cycles;
            while(ch1.freqTimer <= 0) {
                uint16_t period = static_cast<uint16_t>(ch1.NR1x[3] | ((ch1.NR1x[4] & 0x07) << 8));
                ch1.freqTimer += (2048 - period) * 4;
                ch1.dutyStep = (ch1.dutyStep + 1) % 8;
            }
        }

        if(ch2.enable) {
            ch2.freqTimer -= cycles;
            while(ch2.freqTimer <= 0) {
                uint16_t period = static_cast<uint16_t>(ch2.NR2x[2] | ((ch2.NR2x[3] & 0x07) << 8));
                ch2.freqTimer += (2048 - period) * 4;
                ch2.dutyStep = (ch2.dutyStep + 1) % 8;
            }
        }

        if(ch3.enable) {
            int left = cycles;
            while(left > 0) {
                int step = std::min(left, std::max(1, ch3.freqTimer));
                ch3.freqTimer -= step;
                ch3.sinceRead += step;
                left -= step;
                if(ch3.freqTimer <= 0) {
                    uint16_t period = static_cast<uint16_t>(ch3.NR3x[3] | ((ch3.NR3x[4] & 0x07) << 8));
                    ch3.freqTimer += (2048 - period) * 2;
                    ch3.samplePosition = (ch3.samplePosition + 1) % 32;
                    ch3.sinceRead = 0;
                }
            }
        }

        if(ch4.enable) {
            ch4.freqTimer -= cycles;
            while(ch4.freqTimer <= 0) {
                ch4.freqTimer += noisePeriod();
                uint8_t xorResult = (ch4.lfsr & 0x01) ^ ((ch4.lfsr >> 1) & 0x01);
                ch4.lfsr >>= 1;
                ch4.lfsr |= static_cast<uint16_t>(xorResult << 14); 
                
                if((ch4.NR4x[2] >> 3) & 0x01) {
                    ch4.lfsr &= static_cast<uint16_t>(~(1 << 6));
                    ch4.lfsr |= static_cast<uint16_t>(xorResult << 6);
                }
            }
        }
    }

    const int SAMPLE_RATE = 48000;
    downsampleAccumulator += SAMPLE_RATE * cycles; 
    if(downsampleAccumulator >= 4194304) { //send help
        downsampleAccumulator -= 4194304; 
        uint8_t s1 = 0, s2 = 0, s3 = 0, s4 = 0;

        if(ch1.enable) {
            uint8_t duty = (ch1.NR1x[1] >> 6) & 0x03;
            if (dutyPatterns[duty][ch1.dutyStep]) s1 = ch1.volume; 
        }

        if(ch2.enable) {
            uint8_t duty = (ch2.NR2x[0] >> 6) & 0x03;
            if (dutyPatterns[duty][ch2.dutyStep]) s2 = ch2.volume;
        }

        if(ch3.enable && ch3.outputLevel != 0) {
            uint8_t byteIdx = static_cast<uint8_t>(ch3.samplePosition / 2);
            uint8_t nibble = ch3.waveRAM[byteIdx];
            if (ch3.samplePosition % 2 == 0) nibble >>= 4;
            else nibble &= 0x0F;
            s3 = static_cast<uint8_t>(nibble >> (ch3.outputLevel - 1));
        }

        if(ch4.enable) {
            if((ch4.lfsr & 0x01) == 0) s4 = ch4.volume;
        }

        uint8_t nr51 = NR5x[1];
        uint8_t rightTotal = 0, leftTotal = 0;

        if(nr51 & 0x01) rightTotal += s1;
        if(nr51 & 0x02) rightTotal += s2;
        if(nr51 & 0x04) rightTotal += s3;
        if(nr51 & 0x08) rightTotal += s4;

        if(nr51 & 0x10) leftTotal += s1;
        if(nr51 & 0x20) leftTotal += s2;
        if(nr51 & 0x40) leftTotal += s3;
        if(nr51 & 0x80) leftTotal += s4;

        uint8_t nr50 = NR5x[0];
        uint8_t rightVol = (nr50 & 0x07) + 1;
        uint8_t leftVol = ((nr50 >> 4) & 0x07) + 1;

        int finalRight = rightTotal * rightVol;
        int finalLeft = leftTotal * leftVol;

        float inL = static_cast<float>(finalLeft) / 480.0f;
        float outLeft = inL - capL;
        capL = inL - outLeft * 0.9963f;

        float inR = static_cast<float>(finalRight) / 480.0f;
        float outRight = inR - capR;
        capR = inR - outRight * 0.9963f;

        audioBuffer.push_back(outLeft);
        audioBuffer.push_back(outRight);
    }
}






























//ts took me so long, i'm gonna go insane, idk how i did it but it works now, the code works but my brain doesn't work anymore
//why did i do this
//am i a masochist?