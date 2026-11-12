#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <malloc.h>
#include <unistd.h>
#include <sysutil/video.h>
#include <rsx/rsx.h>
#include <rsx/gcm_sys.h>
#include "screen.h"
#define CB_SIZE   0x100000
#define HOST_SIZE (32 * 1024 * 1024)
#define NBUF 2

static gcmContextData *ctx = nullptr;
static uint32_t *fb[NBUF];
static uint32_t fbOffset[NBUF];
static int curBuf = 0;
static int scrW = 0, scrH = 0;
static bool firstFlip = true;

static void waitFlip()
{
    while (gcmGetFlipStatus() != 0)
        usleep(200);
    gcmResetFlipStatus();
}

bool screenInit()
{
    void *host = memalign(1024 * 1024, HOST_SIZE);
    rsxInit(&ctx, CB_SIZE, HOST_SIZE, host);
    if (ctx == nullptr) { printf("rsxInit fallo\n"); return false; }
 videoState state;
    videoResolution res;
    if (videoGetState(0, 0, &state) != 0 || state.state != 0) { printf("videoGetState fallo\n"); return false; }
    if (videoGetResolution(state.displayMode.resolution, &res) != 0) { printf("videoGetResolution fallo\n"); return false; }

    videoConfiguration vconfig;
    memset(&vconfig, 0, sizeof(vconfig));
    vconfig.resolution = state.displayMode.resolution;
    vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
    vconfig.pitch = res.width * sizeof(uint32_t);
    vconfig.aspect = state.displayMode.aspect;
    if (videoConfigure(0, &vconfig, nullptr, 0) != 0) { printf("videoConfigure fallo\n"); return false; }
    if (videoGetState(0, 0, &state) != 0) { printf("videoGetState2 fallo\n"); return false; }

    gcmSetFlipMode(GCM_FLIP_VSYNC);
    scrW = res.width;
    scrH = res.height;

    for (int i = 0; i < NBUF; i++)
    {
        const int pitch = scrW * sizeof(uint32_t);
        fb[i] = (uint32_t *)rsxMemalign(64, pitch * scrH);
        if (fb[i] == nullptr) { printf("rsxMemalign fallo\n"); return false; }
        if (rsxAddressToOffset(fb[i], &fbOffset[i]) != 0) { printf("rsxAddressToOffset fallo\n"); return false; }
        if (gcmSetDisplayBuffer(i, fbOffset[i], pitch, scrW, scrH) != 0) { printf("gcmSetDisplayBuffer fallo\n"); return false; }
        memset(fb[i], 0, pitch * scrH);
    }
    gcmResetFlipStatus();
    printf("pantalla %dx%d lista\n", scrW, scrH);
    return true;
}

int screenWidth() { return scrW; }
int screenHeight() { return scrH; }
uint32_t *screenBackBuffer() { return fb[curBuf]; }

void screenPresent()
{
 // Mismo orden que el sample cairo: esperar el flip anterior antes de pedir otro.
    if (!firstFlip)
        waitFlip();
    firstFlip = false;
    if (gcmSetFlip(ctx, curBuf) == 0)
    {
        rsxFlushBuffer(ctx);
        gcmSetWaitFlip(ctx);
    }
    curBuf = (curBuf + 1) % NBUF;
}
