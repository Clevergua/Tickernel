#pragma once

#include <tickernelCore.h>
#include <graphicEngine.h>
#include <luaEngine.h>
typedef struct TickernelEngineStruct
{
    int targetFrameRate;
    uint32_t frameCount;
    bool canTick;
    char *assetsPath;

    GraphicEngine *pGraphicEngine;
    RenderPipeline *pDeferredRenderPipeline;
    LuaEngine *pLuaEngine;
} TickernelEngine;

void RunTickernelEngine(TickernelEngine *pTickernelEngine);
