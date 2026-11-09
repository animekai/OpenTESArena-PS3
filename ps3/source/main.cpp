#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <SDL.h>

#include "components/vfs/manager.hpp"
#include "Assets/IMGFile.h"
#include "Assets/COLFile.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("Arena PS3 etapa1: inicio\n");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init fallo: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL_Init ok\n");

    SDL_Window *window = SDL_CreateWindow("Arena", 0, 0, 1280, 720, 0);
    printf("ventana: %p\n", (void*)window);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    printf("renderer: %p\n", (void*)renderer);
    if (window == nullptr || renderer == nullptr)
    {
        printf("Ventana/renderer fallo: %s\n", SDL_GetError());
        return 1;
    }

    VFS::Manager::get().initialize(std::string("/dev_hdd0/ARENA"));
    printf("VFS inicializado\n");

    IMGFile img;
    COLFile col;
    const bool okImg = img.init("TITLE.IMG");
    const bool okCol = col.init("PAL.COL");
    printf("TITLE.IMG: %d, PAL.COL: %d\n", okImg, okCol);

    SDL_Texture *texture = nullptr;
    if (okImg && okCol)
    {
        const int w = img.getWidth();
        const int h = img.getHeight();
        printf("Imagen %dx%d\n", w, h);

        const Palette &pal = col.getPalette();
        const uint8_t *src = img.getPixels();
        std::vector<uint32_t> pixels(w * h);
        for (int i = 0; i < w * h; i++)
        {
            const Color &c = pal[src[i]];
            pixels[i] = (0xFFu << 24) | (c.r << 16) | (c.g << 8) | c.b;
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STATIC, w, h);
        if (texture != nullptr)
            SDL_UpdateTexture(texture, nullptr, pixels.data(), w * 4);
        printf("textura: %p\n", (void*)texture);
    }

    int frames = 0;
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) running = false;

        if (texture != nullptr) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255); // rojo = fallo de carga
        SDL_RenderClear(renderer);
        if (texture != nullptr) SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        if (++frames % 120 == 0) printf("frame %d\n", frames);
    }
    return 0;
}
