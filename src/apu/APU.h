#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>

struct Channel1{
    std::array<uint8_t,5> NR1x{}; //NR10 - NR14
    int sweepTimer = 0;
    bool sweepEnabled = false;
    int freqShadowReg = 0;
    bool negatedCalc = false;
    bool enable = false;
    int freqTimer = 0;
    int dutyStep = 0;
    int lengthTimer = 0;
    uint8_t volume = 0;
    int envelopeTimer = 0;
};

struct Channel2{
    std::array<uint8_t,4> NR2x{}; //NR21 - NR24
    bool enable = false;
    int freqTimer = 0;
    int dutyStep = 0;
    int lengthTimer = 0;
    uint8_t volume = 0;
    int envelopeTimer = 0;
};

struct Channel3{
    std::array<uint8_t,16> waveRAM{};
    std::array<uint8_t,5> NR3x{}; //NR30 - NR34
    bool enable = false;
    int lengthTimer = 0;
    uint8_t outputLevel = 0;
    int freqTimer = 0;
    int sinceRead = 1000; 
    int samplePosition = 0;
};

struct Channel4{
    std::array<uint8_t,4> NR4x{}; //NR41 - NR44 
    bool enable = false;
    int lengthTimer = 0;
    uint8_t volume = 0;
    int envelopeTimer = 0;
    int freqTimer = 0;
    uint16_t lfsr = 0x7FFF;
};

class APU{
private:
    Channel1 ch1;
    Channel2 ch2;
    Channel3 ch3;
    Channel4 ch4;

    std::array<uint8_t,3> NR5x{}; //NR50 - NR52
    int audioTimer = 0;
    uint8_t frameSequencer = 0;

    int downsampleAccumulator = 0;
    std::vector<float> audioBuffer; 
    const int dutyPatterns[4][8] = {
        {0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,1,1},
        {0,1,1,1,1,1,1,0}
    };
    //capacitor (high pass filter)
    float capL = 0;
    float capR = 0; 
    static constexpr int WAVE_WINDOW = 2;  
    bool waveAccessible() const { return ch3.enable && ch3.sinceRead < WAVE_WINDOW; }

public:
    APU();
    int sweepCalculate();
    uint8_t readByte(uint16_t addr) const;
    void writeByte(uint16_t addr, uint8_t val);
    void tick(int cycles);
    int noisePeriod() const;
    void lengthWrite(int& lengthTimer, bool& enable, bool wasLenEnabled, uint8_t val, int maxLen);
    const std::vector<float>& getAudioBuffer() const { return audioBuffer; }
    void clearAudioBuffer() { audioBuffer.clear(); }
};