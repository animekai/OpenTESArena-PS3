#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <SDL.h>
#include "components/vfs/manager.hpp"
#include "frame_test.h"
#include "Rendering/RenderInitSettings.h"
#include "screen.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("Arena PS3 etapa2 rsx: inicio\n");
    SDL_Init(SDL_INIT_TIMER);
    if (!screenInit()) { printf("screenInit fallo\n"); return 1; }
    VFS::Manager::get().initialize(std::string("/dev_hdd0/ARENA"));
    constexpr int W = 320, H = 200;
    SoftwareRenderer renderer;
    RenderInitSettings init;
    init.init(nullptr, "", [](const Window*) { return Int2(W, H); }, 1, DitheringMode::None);
    if (!renderer.init(init)) { printf("renderer.init fallo\n"); return 1; }
    TestScene scene;
    if (!buildTestScene(renderer, scene)) { printf("escena fallo\n"); return 1; }
    printf("escena lista\n");
    std::vector<uint32_t> out(W * H, 0);
    for (int frame = 0; ; frame++)
    {
        const double yaw = (frame % 360) * 1.0;
        const Uint32 t0 = SDL_GetTicks();
        renderTestFrame(renderer, scene, yaw, out.data());
        const Uint32 t1 = SDL_GetTicks();
        const int sw = screenWidth(), sh = screenHeight();
        uint32_t *dst = screenBackBuffer();
        for (int y = 0; y < sh; y++)
        {
            const uint32_t *srow = out.data() + (y * H / sh) * W;
            uint32_t *drow = dst + y * sw;
            for (int x = 0; x < sw; x++) drow[x] = srow[x * W / sw];
        }
        screenPresent();
        if (frame % 60 == 0) printf("frame %d, submitFrame %u ms\n", frame, (unsigned)(t1 - t0));
    }
    return 0;
}
