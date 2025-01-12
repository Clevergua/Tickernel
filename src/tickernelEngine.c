#include "tickernelEngine.h"

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

void TickernelStart(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Start!\n");
    
    pTickernelEngine->pGraphicContext = TickernelMalloc(sizeof(GraphicContext));
    GraphicContext *pGraphicContext = pTickernelEngine->pGraphicContext;
    pGraphicContext->applicationName = pTickernelEngine->applicationName;
    pGraphicContext->targetPresentMode = pTickernelEngine->targetPresentMode;
    pGraphicContext->targetSwapchainImageCount = pTickernelEngine->targetSwapchainImageCount;
    pGraphicContext->shadersPath = TickernelMalloc(sizeof(char) * FILENAME_MAX);
    strcpy(pGraphicContext->shadersPath, pTickernelEngine->assetsPath);
    TickernelCombinePaths(pGraphicContext->shadersPath, FILENAME_MAX, "shaders");
    StartGraphic(pGraphicContext);
    
    pTickernelEngine->pLuaContext = TickernelMalloc(sizeof(char) * sizeof(LuaContext));
    LuaContext *pLuaContext = pTickernelEngine->pLuaContext;
    pLuaContext->pGraphicContext = pTickernelEngine->pGraphicContext;
    
    pLuaContext->assetPath = TickernelMalloc(FILENAME_MAX);
    strcpy(pLuaContext->assetPath, pTickernelEngine->assetsPath);
    StartLua(pLuaContext);
}

void TickernelUpdate(TickernelEngine *pTickernelEngine)
{
    printf("Tickernel Update!\n");
    struct timespec startTime, endTime;
    timespec_get(&startTime, TIME_UTC);
    UpdateLua(pTickernelEngine->pLuaContext);
    timespec_get(&endTime, TIME_UTC);
    uint32_t luaDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (uint32_t)(endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    printf("Lua Cost Time: %u ms\n", luaDeltaTime);
    
    timespec_get(&startTime, TIME_UTC);
    UpdateGraphic(pTickernelEngine->pGraphicContext);
    timespec_get(&endTime, TIME_UTC);
    uint32_t graphicDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (uint32_t)(endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    printf("Graphic Cost Time: %u ms\n", graphicDeltaTime);
}

void TickernelEnd(TickernelEngine *pTickernelEngine)
{
    LuaContext *pLuaContext = pTickernelEngine->pLuaContext;
    EndLua(pLuaContext);
    TickernelFree(pLuaContext->assetPath);
    TickernelFree(pLuaContext);
    GraphicContext *pGraphicContext = pTickernelEngine->pGraphicContext;
    EndGraphic(pGraphicContext);
    TickernelFree(pGraphicContext->shadersPath);
    TickernelFree(pGraphicContext);
    printf("Tickernel End!\n");
}


