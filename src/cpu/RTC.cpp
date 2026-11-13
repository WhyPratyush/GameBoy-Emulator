#include "RTC.h"

using sys_clock = std::chrono::system_clock;

void RTC::advance1sec() {
    if (live.rtc_s == 59) live.rtc_s = 0;
    else {
        live.rtc_s = (live.rtc_s + 1) & 0x3F; 
        return;
    }

    if (live.rtc_m == 59) live.rtc_m = 0;
    else {
        live.rtc_m = (live.rtc_m + 1) & 0x3F;
        return;
    }

    if(live.rtc_h == 23) live.rtc_h = 0;
    else {
        live.rtc_h = (live.rtc_h + 1) & 0x1F; 
        return;
    }

    uint16_t days = static_cast<uint16_t>(live.rtc_dl) | static_cast<uint16_t>((live.rtc_dh & 0x01) << 8);
    if(days != 511) days++;
    else {
        days = 0;
        live.rtc_dh |= 0x80; 
    }
    live.rtc_dl = static_cast<uint8_t>(days & 0xFF);
    live.rtc_dh = (live.rtc_dh & 0xFE) | static_cast<uint8_t>((days >> 8) & 0x01);
}

void RTC::writeLatch(uint8_t val) {
    if(val == 0x00) latchReady = true;
    else if (val == 0x01 && latchReady) {
        latched = live; 
        latchReady = false;
    } 
    else latchReady = false;
}

uint8_t RTC::readReg(uint8_t reg) const {
    switch(reg) {
        case 0x08: return latched.rtc_s;
        case 0x09: return latched.rtc_m;
        case 0x0A: return latched.rtc_h;
        case 0x0B: return latched.rtc_dl;
        case 0x0C: return latched.rtc_dh;
        default:   return 0xFF;
    }
}

void RTC::writeReg(uint8_t reg, uint8_t val) {
    switch (reg) {
        case 0x08: live.rtc_s  = val & 0x3F; break; 
        case 0x09: live.rtc_m  = val & 0x3F; break; 
        case 0x0A: live.rtc_h  = val & 0x1F; break; 
        case 0x0B: live.rtc_dl = val;        break; 
        case 0x0C: live.rtc_dh = val & 0xC1; break; 
    }
}

void RTC::tick(int cycles) {
    if (live.rtc_dh & 0x40) return;

    cycleCounter += cycles;
    while(cycleCounter >= 4194304) {
        cycleCounter -= 4194304;
        advance1sec();
    }
}

std::vector<uint8_t> RTC::serialize() const{
    std::vector<uint8_t> data;
    data.insert(data.begin(),{live.rtc_s,live.rtc_m,live.rtc_h,live.rtc_dl,live.rtc_dh});

    auto now = sys_clock::now();
    uint64_t timestamp = static_cast<uint64_t>(sys_clock::to_time_t(now));
    for (int i = 0; i < 8; i++) data.push_back(static_cast<uint8_t>((timestamp >> (i * 8)) & 0xFF));

    return data;
}

void RTC::deserialize(const std::vector<uint8_t>& data) {
    if(data.size() < 13) return;
    live.rtc_s  = data[0];
    live.rtc_m  = data[1];
    live.rtc_h  = data[2];
    live.rtc_dl = data[3];
    live.rtc_dh = data[4];

    uint64_t timestamp = 0;
    for(int i = 0; i < 8; ++i)
        timestamp |= static_cast<uint64_t>(data[static_cast<size_t>(5 + i)]) << (i * 8);

    if (live.rtc_dh & 0x40) return; 

    auto now = sys_clock::now();
    uint64_t cur = static_cast<uint64_t>(sys_clock::to_time_t(now));
    if (cur <= timestamp || timestamp == 0) return; 

    uint64_t diff = cur - timestamp;

    uint64_t totalSeconds = live.rtc_s + live.rtc_m*60ULL + live.rtc_h*3600ULL
        + (static_cast<uint64_t>(live.rtc_dl) | (static_cast<uint64_t>(live.rtc_dh & 0x01) << 8)) * 86400ULL;

    totalSeconds += diff;

    uint64_t days = totalSeconds / 86400;
    live.rtc_s = static_cast<uint8_t>(totalSeconds % 60);
    live.rtc_m = static_cast<uint8_t>((totalSeconds / 60) % 60);
    live.rtc_h = static_cast<uint8_t>((totalSeconds / 3600) % 24);

    if(days > 511) {
        live.rtc_dh |= 0x80;
        days %= 512;
    }
    live.rtc_dl = static_cast<uint8_t>(days & 0xFF);
    live.rtc_dh = static_cast<uint8_t>((live.rtc_dh & 0xFE) | ((days >> 8) & 0x01));
}