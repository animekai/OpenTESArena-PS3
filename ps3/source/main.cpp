#include <cstdio>
#include <string>
#include <vector>
#include <SDL.h>
#include "components/vfs/manager.hpp"
#include "frame_test.h"
#include "Rendering/RenderInitSettings.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("Arena PS3 etapa2: inicio\n");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { printf("SDL_Init fallo\n"); return 1; }
    SDL_Window *window = SDL_CreateWindow("Arena", 0, 0, 1280, 720, 0);
    SDL_Renderer *sdl = SDL_CreateRenderer(window, -1, 0);
    VFS::Manager::get().initialize(std::string("/dev_hdd0/ARENA"));
    printf("VFS ok\n");

    constexpr int W = 320, H = 200;
    SoftwareRenderer renderer;
    RenderInitSettings init;
    init.init(nullptr, "", [](const Window*) { return Int2(W, H); }, 1, DitheringMode::None);
    if (!renderer.init(init)) { printf("renderer.init fallo\n"); return 1; }
    printf("renderer ok\n");

    TestScene scene;
    if (!buildTestScene(renderer, scene)) { printf("escena fallo\n"); return 1; }

    SDL_Texture *tex = SDL_CreateTexture(sdl, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W, H);
    std::vector<uint32_t> out(W * H, 0);
    int frame = 0;
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = false;

        const double yaw = (frame % 360) * 1.0;
        const Uint32 t0 = SDL_GetTicks();
        renderTestFrame(renderer, scene, yaw, out.data());
        const Uint32 t1 = SDL_GetTicks();

        SDL_UpdateTexture(tex, nullptr, out.data(), W * 4);
        SDL_RenderClear(sdl);
        SDL_RenderCopy(sdl, tex, nullptr, nullptr);
        SDL_RenderPresent(sdl);
        if (frame % 30 == 0) printf("frame %d yaw %.0f submitFrame %u ms\n", frame, yaw, (unsigned)(t1 - t0));
        frame++;
    }
    return 0;
}
