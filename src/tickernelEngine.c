#include "tickernelEngine.h"

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

void tickernelStart(uint32_t targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight, TickernelEngine **ppTickernelEngine)
{
    TickernelEngine *pTickernelEngine = tickernelMalloc(sizeof(TickernelEngine));
    pTickernelEngine->frameCount = 0;
    pTickernelEngine->targetSwapchainImageCount = targetSwapchainImageCount;
    pTickernelEngine->targetPresentMode = targetPresentMode;
    createGraphicsContext(targetSwapchainImageCount, targetPresentMode, vkInstance, vkSurface, swapchainWidth, swapchainHeight, &pTickernelEngine->graphicsContext);
    *ppTickernelEngine = pTickernelEngine;
}

void tickernelUpdate(TickernelEngine *pTickernelEngine, uint32_t swapchainWidth, uint32_t swapchainHeight)
{
    pTickernelEngine->frameCount++;
    updateGraphicsContext(&pTickernelEngine->graphicsContext, swapchainWidth, swapchainHeight);
}

void tickernelEnd(TickernelEngine *pTickernelEngine)
{
    destroyGraphicsContext(&pTickernelEngine->graphicsContext);
    tickernelFree(pTickernelEngine);
}
