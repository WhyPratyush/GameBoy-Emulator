#pragma once
#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

class Renderer {
    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
        std::array<uint32_t, 160*144> pixelBuffer{};

    public:
        Renderer(int scale = 4);
        ~Renderer();
        void show(const std::array<uint8_t, 160*144>& shadeBuffer);
};