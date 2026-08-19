#include "Renderer.h"

Renderer::Renderer(int scale) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("GameBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,160 * scale, 144 * scale, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING, 160, 144);
}

Renderer::~Renderer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::show(const std::array<uint8_t, 160*144>& shadeBuffer) {
    static const uint32_t palette[4] = { 0xFFFFFFFF,0xFFAAAAAA,0xFF555555,0xFF000000};

    for (size_t i = 0; i < 160*144; i++) pixelBuffer[i] = palette[shadeBuffer[i] & 0x03];

    SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), 160 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}