#include <algorithm>
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
    unsigned sumMs = 0, maxMs = 0;
    int maxCov = 0;
    for (int frame = 0; ; frame++)
    {
        const double yaw = (frame % 360) * 1.0;
        std::fill(out.begin(), out.end(), 0u);
        const Uint32 t0 = SDL_GetTicks();
        renderTestFrame(renderer, scene, yaw, out.data());
        const unsigned ms = SDL_GetTicks() - t0;
        sumMs += ms;
        if (ms > maxMs) maxMs = ms;
        int cov = 0;
        for (int i = 0; i < W * H; i += 7) if (out[i] != 0) cov++;
        if (cov > maxCov) maxCov = cov;
        const int sw = screenWidth(), sh = screenHeight();
        uint32_t *dst = screenBackBuffer();
        for (int y = 0; y < sh; y++)
        {
            const uint32_t *srow = out.data() + (y * H / sh) * W;
            uint32_t *drow = dst + y * sw;
            for (int x = 0; x < sw; x++) drow[x] = srow[x * W / sw];
        }
        if (frame < 360 && frame % 15 == 0) printf("yaw %.0f: %u ms, pantalla cubierta %d%%\n", yaw, ms, cov * 7 * 100 / (W * H));
        screenPresent();
        if (frame % 120 == 119)
        {
            printf("frames %d-%d: media %.1f ms, max %u ms, cobertura max %d%%\n", frame - 119, frame, sumMs / 120.0, maxMs, maxCov * 7 * 100 / (W * H));
            sumMs = 0; maxMs = 0; maxCov = 0;
        }
    }
    return 0;
}
