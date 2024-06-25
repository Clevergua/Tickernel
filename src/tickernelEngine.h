#pragma once

#include <tickernelCore.h>
#include <gfxDevice.h>
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canTick;
    char *assetsPath;
    GFXDevice *pGFXDevice;
} TickernelEngine;

void RunTickernelEngine(TickernelEngine *pTickernelEngine);
