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

    pTickernelEngine->pGraphicsContext = createGraphicsContext(assetsPath, targetSwapchainImageCount, targetPresentMode, vkInstance, vkSurface, swapchainWidth, swapchainHeight);
    return pTickernelEngine;
}

void tickernelUpdate(TickernelEngine *pTickernelEngine, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    pTickernelEngine->frameCount++;
    updateGraphicsContext(pTickernelEngine->pGraphicsContext, swapchainWidth, swapchainHeight);
}

void tickernelEnd(TickernelEngine *pTickernelEngine)
{
    GraphicsContext *pGraphicsContext = pTickernelEngine->pGraphicsContext;
    destroyGraphicsContext(pGraphicsContext);
    tickernelFree(pTickernelEngine);
    printf("Tickernel End!\n");
}
