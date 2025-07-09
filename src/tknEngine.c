#include "tknEngine.h"

void createTknEngine(uint32_t targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, TknEngine **ppTknEngine)
{
    GraphicsContext graphicsContext;
    createGraphicsContext(targetSwapchainImageCount, targetPresentMode, vkInstance, vkSurface, swapchainExtent, &graphicsContext);

    tknMalloc(sizeof(TknEngine), (void **)ppTknEngine);
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
