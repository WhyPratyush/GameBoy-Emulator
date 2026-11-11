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
    gb.loadRom(romData,argv[1]);

    SDL_InitSubSystem(SDL_INIT_AUDIO); 
    
    SDL_AudioSpec desiredSpec = {};
    desiredSpec.freq = 48000;
    desiredSpec.format = AUDIO_F32; 
    desiredSpec.channels = 2;       
    desiredSpec.samples = 1024;

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, NULL, 0);
    if(audioDevice == 0) {
        std::cerr<<"Audio open failed: "<<SDL_GetError()<<"\n";
    }
    else {
        std::vector<float> prime(3200, 0.0f); 
        SDL_QueueAudio(audioDevice, prime.data(), static_cast<Uint32>(prime.size() * sizeof(float)));
        SDL_PauseAudioDevice(audioDevice, 0);
    }

    Renderer rd;
    bool isRunning = true;
    SDL_Event event;

    using frame_duration = std::chrono::duration<int64_t, std::ratio<70224, 4194304>>;
    using clock = std::chrono::steady_clock;
    auto start = clock::now();
    auto lastSaveTime = clock::now();

    while(isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            else if(event.type == SDL_KEYDOWN) gb.joypad(event.key.keysym.sym, false);
            else if(event.type == SDL_KEYUP) gb.joypad(event.key.keysym.sym, true);
        }
        gb.stepFrame();

        const auto& audioData = gb.getAudioBuffer();
        if(!audioData.empty() && SDL_GetQueuedAudioSize(audioDevice) < 4096 * sizeof(float)) {
            SDL_QueueAudio(audioDevice, audioData.data(), static_cast<Uint32>(audioData.size() * sizeof(float)));
        }
        gb.clearAudioBuffer();

        if(gb.pollFrameReady()) {
            rd.show(gb.getFrameBuffer());
            auto curTime = clock::now();
            if(curTime - lastSaveTime >= std::chrono::seconds(1)) {
                gb.saveBattery(false);
                lastSaveTime = curTime;
            }
        }
        start += std::chrono::duration_cast<clock::duration>(frame_duration(1));
        std::this_thread::sleep_until(start);
    }

    SDL_CloseAudioDevice(audioDevice);

    gb.saveBattery(true);
    return 0;
}