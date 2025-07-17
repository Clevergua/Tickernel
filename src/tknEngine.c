#include "tknEngine.h"

void createTknEnginePtr(uint32_t targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, TknEngine **ppTknEngine)
{
    GfxContext gfxContext;
    createGfxContext(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent, &gfxContext);

    *ppTknEngine = tknMalloc(sizeof(TknEngine));
    **ppTknEngine = (TknEngine){
        .frameCount = 0,
        .gfxContext = gfxContext,
    };
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
