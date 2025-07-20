#include "tknEngine.h"

TknEngine *createTknEnginePtr(uint32_t targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent)
{
    TknEngine *pTknEngine = tknMalloc(sizeof(TknEngine));
    *pTknEngine = (TknEngine){
        .frameCount = 0,
        .gfxContext = createGfxContext(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent),
    };
    return pTknEngine;
}

void updateTknEnginePtr(TknEngine *pTknEngine, VkExtent2D swapchainExtent)
{
    pTknEngine->frameCount++;
    if (pTknEngine->frameCount == UINT32_MAX)
    {
        tknError("Frame count overflowed!\n");
    }
    else
    {
        updateGfxContext(&pTknEngine->gfxContext, swapchainExtent);
    }
}

void destroyTknEnginePtr(TknEngine *pTknEngine)
{
    destroyGfxContext(pTknEngine->gfxContext);
    tknFree(pTknEngine);
}
