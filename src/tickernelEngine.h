#pragma once
#include <luaEngine.h>
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canUpdate;
    char *assetsPath;

    GraphicEngine *pGraphicEngine;
    LuaEngine *pLuaEngine;
} TickernelEngine;

void RunTickernelEngine(TickernelEngine *pTickernelEngine);
