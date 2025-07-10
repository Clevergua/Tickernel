#include "tknEngine.h"

void createTknEngine(uint32_t targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, TknEngine **ppTknEngine)
{
    GraphicsContext graphicsContext;
    createGraphicsContext(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent, &graphicsContext);

    *ppTknEngine = tknMalloc(sizeof(TknEngine));
    **ppTknEngine = (TknEngine){
        .frameCount = 0,
        .graphicsContext = graphicsContext,
    };
}

void updateTknEngine(TknEngine *pTknEngine, VkExtent2D swapchainExtent)
{
    pTknEngine->frameCount++;
    if (pTknEngine->frameCount == UINT32_MAX)
    {
        tknError("Frame count overflowed!\n");
    }
    else
    {
        updateGraphicsContext(&pTknEngine->graphicsContext, swapchainExtent);
    }
}

void destroyTknEngine(TknEngine *pTknEngine)
{
    destroyGraphicsContext(pTknEngine->graphicsContext);
    tknFree(pTknEngine);
}
