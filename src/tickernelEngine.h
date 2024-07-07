#pragma once

#include <tickernelCore.h>
#include <gfxEngine.h>
#include <luaEngine.h>
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canTick;
    char *assetsPath;

    GFXEngine *pGFXEngine;
    LuaEngine *pLuaEngine;
} TickernelEngine;

void RunTickernelEngine(TickernelEngine *pTickernelEngine);
