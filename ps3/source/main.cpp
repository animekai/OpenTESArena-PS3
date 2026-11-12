#include <cstdio>
#include <cstdint>
#include <vector>
#include <SDL.h>

static const bool USE_SURFACE = false;
static const int DELAY_MS = 100;

int main(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("sdltest: inicio surface=%d delay=%d\n", (int)USE_SURFACE, DELAY_MS);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { printf("SDL_Init fallo\n"); return 1; }
    SDL_Window *window = SDL_CreateWindow("Arena", 0, 0, 1280, 720, 0);
    constexpr int W = 320, H = 200;
    std::vector<uint32_t> out(W * H, 0);

    SDL_Renderer *sdl = nullptr;
    SDL_Texture *tex = nullptr;
    SDL_Surface *src = nullptr;
    if (USE_SURFACE)
        src = SDL_CreateRGBSurfaceFrom(out.data(), W, H, 32, W * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    else
    {
        sdl = SDL_CreateRenderer(window, -1, 0);
        tex = SDL_CreateTexture(sdl, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W, H);
    }

    int frame = 0;
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = false;

        for (int i = 0; i < W * H; i++)
            out[i] = 0xFF000000u | ((unsigned)((frame * 4) & 255) << 16) | 0x4000u;

        const bool log = (frame % 64 == 0) || (frame >= 250 && frame <= 262);
        if (log) printf("frame %d\n", frame);

        if (USE_SURFACE)
        {
            SDL_Surface *ws = SDL_GetWindowSurface(window);
            SDL_BlitScaled(src, nullptr, ws, nullptr);
            SDL_UpdateWindowSurface(window);
        }
        else
        {
            SDL_UpdateTexture(tex, nullptr, out.data(), W * 4);
            SDL_RenderClear(sdl);
            SDL_RenderCopy(sdl, tex, nullptr, nullptr);
            SDL_RenderPresent(sdl);
        }
        if (log) printf("   ok %d\n", frame);
        if (DELAY_MS > 0) SDL_Delay(DELAY_MS);
        frame++;
    }
    return 0;
}
