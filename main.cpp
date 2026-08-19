#include <iostream>
#include <fstream>
#include <vector>
#include "GameBoy.h"
#include "Renderer.h"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <rom_path>\n";
        return 1;
    }

    std::ifstream romFile(argv[1], std::ios::binary | std::ios::ate);
    if(!romFile.is_open()) {
        std::cerr<<"Failed to open ROM: "<<argv[1]<<"\n";
        return 1;
    }

    std::streamsize size = romFile.tellg();
    romFile.seekg(0, std::ios::beg);

    std::vector<uint8_t> romData(static_cast<size_t>(size));
    if(!romFile.read(reinterpret_cast<char*>(romData.data()), size)) {
        std::cerr<<"Failed to read ROM data\n";
        return 1;
    }

    GameBoy gb;
    gb.loadRom(romData);

    Renderer rd;
    bool isRunning = true;
    SDL_Event event;

    using frame_duration = std::chrono::duration<int64_t, std::ratio<70224, 4194304>>;
    using clock = std::chrono::steady_clock;
    auto start = clock::now();

    while(isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            else if(event.type == SDL_KEYDOWN) gb.joypad(event.key.keysym.sym, false);
            else if(event.type == SDL_KEYUP) gb.joypad(event.key.keysym.sym, true);
        }
        gb.stepFrame();
        if(gb.pollFrameReady()) rd.show(gb.getFrameBuffer());
        start += std::chrono::duration_cast<clock::duration>(frame_duration(1));
        std::this_thread::sleep_until(start);
    }

    return 0;
}