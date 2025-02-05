#include "tickernelEngine.h"

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

TickernelEngine *tickernelStart(const char *assetsPath, uint32_t targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface,uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    printf("Tickernel Start!\n");
    TickernelEngine *pTickernelEngine = tickernelMalloc(sizeof(TickernelEngine));
    pTickernelEngine->frameCount = 0;
    pTickernelEngine->assetsPath = assetsPath;
    pTickernelEngine->targetSwapchainImageCount = targetSwapchainImageCount;
    pTickernelEngine->targetPresentMode = targetPresentMode;
    pTickernelEngine->vkInstance = vkInstance;
    pTickernelEngine->vkSurface = vkSurface;

    pTickernelEngine->pGraphicContext = startGraphic(assetsPath, targetSwapchainImageCount, targetPresentMode, vkInstance, vkSurface, swapchainWidth, swapchainHeight);
    pTickernelEngine->pLuaContext = startLua(assetsPath, pTickernelEngine->pGraphicContext);

    return pTickernelEngine;
}

void tickernelUpdate(TickernelEngine *pTickernelEngine, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    // printf("Tickernel Update!\n");
    // struct timespec startTime, endTime;
    // timespec_get(&startTime, TIME_UTC);
    UpdateLua(pTickernelEngine->pLuaContext);
    // timespec_get(&endTime, TIME_UTC);
    // uint32_t luaDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (uint32_t)(endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    // printf("Lua Cost Time: %u ms\n", luaDeltaTime);

    // timespec_get(&startTime, TIME_UTC);
    updateGraphic(pTickernelEngine->pGraphicContext, swapchainWidth, swapchainHeight);
    // timespec_get(&endTime, TIME_UTC);
    // uint32_t graphicDeltaTime = (uint32_t)(endTime.tv_sec - startTime.tv_sec) * MILLISECONDS_PER_SECOND + (uint32_t)(endTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    // printf("Graphic Cost Time: %u ms\n", graphicDeltaTime);
}

void tickernelEnd(TickernelEngine *pTickernelEngine)
{
    LuaContext *pLuaContext = pTickernelEngine->pLuaContext;
    endLua(pLuaContext);
    GraphicContext *pGraphicContext = pTickernelEngine->pGraphicContext;
    endGraphic(pGraphicContext);
    tickernelFree(pTickernelEngine);
    printf("Tickernel End!\n");
}
