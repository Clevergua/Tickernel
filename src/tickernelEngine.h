#pragma once
#include <luaBinding.h>
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canUpdate;
    char *assetsPath;

    GraphicContext *pGraphicContext;
    LuaEngine *pLuaEngine;
} TickernelEngine;

void RunTickernelEngine(TickernelEngine *pTickernelEngine);
