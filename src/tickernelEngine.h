#pragma once
#include <luaEngine.h>
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canTick;
    char *assetsPath;

    GraphicEngine *pGraphicEngine;
    LuaEngine *pLuaEngine;
} TickernelEngine;

void RunTickernelEngine(TickernelEngine *pTickernelEngine);
